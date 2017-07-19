use ref_counted_vec::RefCountedVec;
use std;

//////////////////////////////////////////////////////////////////////////////

fn get_bit(value: usize, bit: usize) -> bool
{
    (value >> bit) & 1 != 0
}

#[derive (Copy, Clone)]
pub struct NCDimNode {
    left: Option<usize>,
    right: Option<usize>,
    next: Option<usize>
}

pub struct NCDim {
    nodes: RefCountedVec<NCDimNode>,
    width: usize
}

impl NCDim {
    fn len(&self) -> usize {
        return self.nodes.len();
    }

    fn new(width: usize) -> NCDim {
        NCDim {
            nodes: RefCountedVec::new(),
            width: width
        }
    }
}

impl NCDim {
    pub fn at(&self, index: usize) -> &NCDimNode {
        return &self.nodes.values[index];
    }

    pub fn at_mut(&mut self, index: usize) -> &mut NCDimNode {
        return &mut self.nodes.values[index];
    }
}

pub struct Nanocube<Summary> {
    base_root: Option<usize>,
    dims: Vec<NCDim>,
    summaries: RefCountedVec<Summary>
}

// we're not using add here because I can't make the trait implementations
// and declarations work like I want them to.
pub trait Monoid<RHS = Self> {
    fn mapply(&self, rhs: &RHS) -> Self;
}

impl Monoid for usize {
    fn mapply(&self, rhs: &usize) -> usize { self + rhs }
}


impl <Summary: Monoid + PartialOrd> Nanocube<Summary> {

    pub fn new(widths: Vec<usize>) -> Nanocube<Summary> {
        assert!(widths.len() > 0);
        Nanocube {
            base_root: None,
            dims: widths.into_iter().map(|w| NCDim::new(w)).collect(),
            summaries: RefCountedVec::new()
        }
    }
    
    pub fn make_node_ref(&mut self, node_index: Option<usize>, dim: usize) -> usize {
        match node_index {
            None => 0,
            Some(v) if dim == self.dims.len() => self.summaries.make_ref(v),
            Some(v) /* otherwise */           => {
                assert!(v < self.dims[dim].len());
                self.dims[dim].nodes.make_ref(v)
            }
        }
    }

    pub fn release_node_ref(&mut self, node_index: Option<usize>, dim: usize) {
        if let Some(node_index) = node_index {
            let mut stack = Vec::<(usize, usize)>::new();
            stack.push((node_index, dim));
            while stack.len() > 0 {
                let (node_index, dim) = stack.pop().expect("internal error");
                if dim == self.dims.len() {
                    self.summaries.release_ref(node_index);
                } else {
                    let new_ref_count = self.dims[dim].nodes.release_ref(node_index);
                    if new_ref_count == 0 {
                        let mut node = self.dims[dim].at_mut(node_index);
                        if let Some(left) = node.left {
                            stack.push((left, dim));
                        }
                        if let Some(right) = node.right {
                            stack.push((right, dim));
                        }
                        if let Some(next) = node.next {
                            stack.push((next, dim+1));
                        }
                        node.left = None;
                        node.right = None;
                        node.next = None;
                    }
                }
            }
        }
    }
    
    pub fn insert_fresh_node(&mut self,
                             summary: Summary, addresses: &Vec<usize>, dim: usize, bit: usize)
                             -> (Option<usize>, Option<usize>) {
        assert!(self.dims.len() == addresses.len());
        if dim == self.dims.len() {
            let new_ref = self.summaries.insert(summary);
            (Some(new_ref), Some(new_ref))
        } else {
            let width = self.dims[dim].width;
            if bit == width {
                let next_dim_result = self.insert_fresh_node(summary, addresses, dim+1, 0);
                let new_node_at_current_dim = NCDimNode {
                    left: None,
                    right: None,
                    next: next_dim_result.0
                };
                self.make_node_ref(next_dim_result.0, dim+1);
                let new_index = self.dims[dim].nodes.insert(new_node_at_current_dim);
                return (Some(new_index), next_dim_result.1);
            } else {
                let where_to_insert = get_bit(addresses[dim], width-bit-1);
                let recursion_result = self.insert_fresh_node(summary, addresses, dim, bit+1);
                let left_recursion_result = recursion_result.0.expect("fresh node should have returned Some");
                let next = self.dims[dim].at(left_recursion_result).next;
                let new_refinement_node = NCDimNode {
                    left:  if !where_to_insert { recursion_result.0 } else { None },
                    right: if  where_to_insert { recursion_result.0 } else { None },
                    next:  next
                };
                self.make_node_ref(next, dim+1);
                self.make_node_ref(recursion_result.0, dim);
                let new_index = self.dims[dim].nodes.insert(new_refinement_node);
                return (Some(new_index), recursion_result.1);
            }
        }
    }

    pub fn get_node(&self, dim: usize, node_index: usize) -> NCDimNode {
        self.dims[dim].at(node_index).clone()
    }
    
    pub fn merge(&mut self,
                 node_1: Option<usize>, node_2: Option<usize>,
                 dim: usize) -> (Option<usize>, Option<usize>) {
        if let None = node_1 {
            return (node_2, self.get_summary_index(node_2, dim));
        }
        if let None = node_2 {
            return (node_1, self.get_summary_index(node_1, dim));
        }
        let node_1 = node_1.expect("internal error");
        let node_2 = node_2.expect("internal error");
        if dim == self.dims.len() {
            let new_summary = {
                let ref node_1_summary = self.summaries.values[node_1];
                let ref node_2_summary = self.summaries.values[node_2];
                node_1_summary.mapply(node_2_summary)
            };
            let new_summary_index = self.summaries.insert(new_summary);
            return (Some(new_summary_index), Some(new_summary_index));
        }
        let node_1_node = self.get_node(dim, node_1);
        let node_2_node = self.get_node(dim, node_2);
        let left_merge = self.merge(node_1_node.left, node_2_node.left, dim);
        let right_merge = self.merge(node_1_node.right, node_2_node.right, dim);
        let node_1_index = left_merge.0;
        let node_2_index = right_merge.0;
        let next_merge = match (node_1_index, node_2_index) {
            (None, None) =>
                self.merge(node_1_node.next, node_2_node.next, dim+1),
            (None, Some(node_2_merge_next)) =>
                (self.dims[dim].at(node_2_merge_next).next, right_merge.1),
            (Some(node_1_merge_next), None) =>
                (self.dims[dim].at(node_1_merge_next).next, left_merge.1),
            (Some(node_1_merge_next), Some(node_2_merge_next)) => {
                let node_1_merge_next_next = self.get_node(dim, node_1_merge_next).next;
                let node_2_merge_next_next = self.get_node(dim, node_2_merge_next).next;
                self.merge(node_1_merge_next_next,
                           node_2_merge_next_next,
                           dim+1)
            }
        };
        let new_node = NCDimNode {
            left: left_merge.0,
            right: right_merge.0,
            next: next_merge.0
        };
        self.make_node_ref(left_merge.0, dim);
        self.make_node_ref(right_merge.0, dim);
        self.make_node_ref(next_merge.0, dim);
        (Some(self.dims[dim].nodes.insert(new_node)), next_merge.1)
    }

    pub fn insert(&mut self,
                  summary: Summary, addresses: &Vec<usize>, dim: usize, bit: usize,
                  current_node_index: Option<usize>) ->
        (Option<usize>, Option<usize>)
    {
        match current_node_index {
            None => self.insert_fresh_node(summary, addresses, dim, bit),
            Some(current_node_index) => {
                let width = self.dims[dim].width;
                let current_node = self.get_node(dim, current_node_index);
                if bit != width {
                    // recurse into current dimension through refinement nodes
                    let current_address = addresses[dim];
                    let where_to_insert = get_bit(current_address, width-bit-1);
                    let refinement_node =
                        if where_to_insert { current_node.right } else { current_node.left };
                    let result = self.insert(summary, addresses, dim, bit+1, refinement_node);
                    self.make_node_ref(result.0, dim);
                    let (fresh_left_node, fresh_right_node) =
                        if where_to_insert {
                            (None, result.0)
                        } else {
                            (result.0, None)
                        };
                    let merged_left  = self.merge(current_node.left, fresh_left_node, dim);
                    let merged_right = self.merge(current_node.right, fresh_right_node, dim);
                    
                    // FIXME it might be better to have at() accept Option<usize> instead
                    // and lift everything up to work with Option<>s
                    let merged_left_next  = self.get_node(dim, merged_left .0.expect("Foo")).next;
                    let merged_right_next = self.get_node(dim, merged_right.0.expect("Foo")).next;
                    let merged_next  = self.merge(merged_left_next,
                                                  merged_right_next,
                                                  dim+1);
                    self.release_node_ref(result.0, dim);
                    let new_node = NCDimNode {
                        left: merged_left.0,
                        right: merged_right.0,
                        next: merged_next.0
                    };
                    let new_index = self.dims[dim].nodes.insert(new_node);
                    return (Some(new_index), merged_next.1);
                } else {
                    let result = self.insert(summary, addresses, dim+1, 0, current_node.next);
                    let new_node = NCDimNode {
                        left: None,
                        right: None,
                        next: result.0
                    };
                    let new_index = self.dims[dim].nodes.insert(new_node);
                    return (Some(new_index), result.1);
                }
            }
        }
    }

    // (&mut self,
    //               summary: Summary, addresses: &Vec<usize>, dim: usize, bit: usize,
    //               current_node_index: Option<usize>) ->
    //     (Option<usize>, Option<usize>)

    pub fn add(&mut self,
               summary: Summary,
               addresses: &Vec<usize>)
    {
        let base = self.base_root;
        let (result_base, _) = self.insert(summary, addresses, 0, 0, base);
        self.base_root = result_base;
    }
    
    // pub fn compact(&mut self) {}

    // pub fn content_compact(&mut self) {}

    pub fn get_summary_index(&self, node_index: Option<usize>, dim: usize) -> Option<usize> {
        let mut current_node_index = node_index;
        let mut dim = dim;
        while dim < self.dims.len() {
            current_node_index = match current_node_index {
                None => {
                    return None;
                }
                Some(i) => {
                    let next_index = self.dims[dim].at(i).next;
                    dim += 1;
                    next_index
                }
            }
        }
        return current_node_index;
    }

    pub fn report_size(&self) {
        println!("Summary counts: {}", self.summaries.len());
        println!("Dimension counts:");
        for dim in self.dims.iter() {
            println!("  {}", dim.len());
        }
        println!("");
    }
}

//////////////////////////////////////////////////////////////////////////////

fn node_id(i: usize, dim: usize) -> String {
    format!("\"{}_{}\"", i, dim)
}

pub fn print_dot_ncdim<W: std::io::Write>(w: &mut W, dim: &NCDim, d: usize, draw_next: bool) -> Result<(), std::io::Error>
{
    writeln!(w, " subgraph cluster_{} {{", d).expect("Can't write to w");
    writeln!(w, " label=\"Dim. {}\";", d).expect("Can't write to w");
    for i in 0..dim.nodes.values.len() {
        let s = match dim.nodes.values[i].next {
            None => format!("{}", "None"),
            Some(s) => format!("{}", s)
        };
        writeln!(w, "  {} [label=\"{}:{}\"];", node_id(i, d), i, s).expect("Can't write to w");;
    }
    writeln!(w, "}}").expect("Can't write to w");;
    for i in 0..dim.nodes.values.len() {
        let node = &dim.nodes.values[i];
        if let Some(left) = node.left {
            writeln!(w, "  {} -> {} [label=\"0\"];", node_id(i, d), node_id(left, d)).expect("Can't write to w");;
        }
        if let Some(right) = node.right {
            writeln!(w, "  {} -> {} [label=\"1\"];", node_id(i, d), node_id(right, d)).expect("Can't write to w");;
        }
    }
    Ok(())
}

pub fn print_dot<W: std::io::Write, Summary>(w: &mut W, nc: &Nanocube<Summary>) -> Result<(), std::io::Error>
{
    writeln!(w, "digraph G {{").expect("Can't write to w");
    writeln!(w, "    splines=line;").expect("Can't write to w");;
    for i in 0..nc.dims.len() {
        print_dot_ncdim(w, &nc.dims[i], i, i < (nc.dims.len() - 1)).expect("Can't write to w");
    }
    writeln!(w, "}}").expect("Can't write to w");
    Ok(())
}

//////////////////////////////////////////////////////////////////////////////

pub fn write_to_disk<Summary>(name: &str, nc: &Nanocube<Summary>) -> Result<(), std::io::Error>
{
    let mut f = std::fs::File::create(name)
        .expect("cannot create file");
    print_dot(&mut f, &nc)
}

pub fn smoke_test()
{
    let mut nc = Nanocube::<usize>::new(vec![2, 2]);
    nc.report_size();
    write_to_disk("out0.dot", &nc).expect("Couldn't write");
    nc.add(1, &vec![0, 0]);
    write_to_disk("out1.dot", &nc).expect("Couldn't write");
    nc.add(1, &vec![3, 3]);
    write_to_disk("out2.dot", &nc).expect("Couldn't write");

    
    println!("{:?}", nc.base_root);
}