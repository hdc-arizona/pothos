#ifndef SATGARBAGECUBE_H_
#define SATGARBAGECUBE_H_

// SATGarbageCube is a GarbageCube whose last dimension is implemented
// as a sparse Summed Area Table. We use persistent vectors to
// implement the SAT to minimize the effect of copying summaries caused by
// the algorithms in GarbageCube.

#include <immutable/array.h>
#include <utility>
#include <iostream>
#include <boost/assert.hpp>
#include "debug.h"
#include "basecube.h"

namespace nc2 {

template <typename T>
struct PersistentSAT
{
  typedef immutable::Array<std::pair<size_t, T> > SAT;
  typedef immutable::ref<SAT> SATRef;

  SATRef sat_;
  
  PersistentSAT(): sat_(SAT::empty()) {}
  PersistentSAT(const PersistentSAT &other): sat_(other.sat_) {}
  explicit PersistentSAT(const SATRef &sat): sat_(sat) {}

  PersistentSAT<T> &operator=(const PersistentSAT<T> &other) {
    if (&other == this)
      return *this;
    sat_ = other.sat_;
    return *this;
  }

  bool operator!=(const PersistentSAT<T> &other) const {
    return !(*this == other);
  }
  bool operator==(const PersistentSAT<T> &other) const{
    TRACE_BLOCK("operator==");
    TRACE(*this);
    TRACE(other);
    int result = sat_->compare(other.sat_);
    TRACE(result);
    return result == 0;
  }

  SATRef add_expr(size_t key, const T &val) const {
    size_t sz = sat_->size();
    if (sz == 0) {
      return sat_->push(std::make_pair(key, val));
    }
    const std::pair<size_t, T> &last = sat_->last();
    if (last.first < key) {
      return sat_->push(std::make_pair(key, last.second + val));
    }

    // optimize for common case: updating the very last value.
    if (last.first == key) {
      return sat_->set(sz-1, std::make_pair(key, last.second + val));
    }

    // This is the slow path, we have a potentially O(n) operation
    // here!

    size_t split_i = lower_bound(key);
    BOOST_ASSERT(split_i < sat_->size());
    size_t split_key = sat_->get(split_i).first;

    TRACE(split_i);
    TRACE(sz);
    auto left  = sat_->slice(0, split_i);
    auto right = sat_->slice(split_i, sz);
    
    right = right->modify(
        [&val, &right] (auto t) {
          auto bb = right->begin();
          for (auto b = right->begin(), e = right->end(); b!=e; ++b) {
            size_t i = b.distanceTo(bb);
            t->set(i, std::make_pair((*b).first, (*b).second + val));
          }
        });

    TRACE("past modify");
    TRACE(split_key);
    TRACE(key);
    if (split_key == key) {
      return left->concat(right);
    } else if (left->size() == 0) {
      return SAT::create({std::make_pair(key, val)})->concat(right);
    } else {
      const T &left_sup = left->last().second;
      auto vec = left->push(std::make_pair(key, left_sup + val))->concat(right);
      return vec;
    }
    BOOST_ASSERT(false);
  }
  
  PersistentSAT<T> add(size_t key, const T &val) const {
    return PersistentSAT<T>(add_expr(key, val));
  }

  void add_mutate(size_t key, const T &val) {
    sat_ = add_expr(key, val);
  }
  
  size_t lower_bound(size_t key) const {
    if (sat_->size() == 0) {
      return 0;
    }
    size_t left_key  = sat_->first().first, left_i = 0;
    size_t right_key = sat_->last().first, right_i = sat_->size()-1;

    if (key <= left_key) {
      return 0;
    }
    if (key > right_key) {
      return sat_->size();
    }
    if (key == right_key) {
      return right_i;
    }

    size_t center_i = left_i + ((right_i - left_i) >> 1),
         center_key = sat_->get(center_i).first;

    while (right_i - left_i > 1) {
      TRACE(left_i);
      TRACE(right_i);
      TRACE(center_i);
      TRACE(left_key);
      TRACE(right_key);
      TRACE(center_key);
      // invariant: left < index < right
      if (center_key == key)
        return center_i;
      if (center_key < key) {
        left_key = center_key;
        left_i   = center_i;
      } else {
        right_key = center_key;
        right_i   = center_i;
      }
      center_i = left_i + ((right_i - left_i) >> 1);
      center_key = sat_->get(center_i).first;
    }
    TRACE(left_i);
    TRACE(right_i);
    TRACE(center_i);
    TRACE(left_key);
    TRACE(right_key);
    TRACE(center_key);
    if (left_key == key)
      return left_i;
    else
      return right_i;
  }
  
  T sum(size_t min_key, size_t max_key) const {
    TRACE(*this);
    TRACE(min_key);
    TRACE(max_key);

    if (sat_->size() == 0)
      return T();
    
    size_t min_lb_i = lower_bound(min_key);
    size_t max_lb_i = lower_bound(max_key);

    TRACE(min_lb_i);
    TRACE(max_lb_i);
    
    if (min_lb_i == max_lb_i)
      return T();

    BOOST_ASSERT(max_lb_i > 0);

    const T &max_v =
        (max_lb_i == sat_->size()) ? sat_->last().second :
        (max_key <= sat_->get(max_lb_i).first) ? sat_->get(max_lb_i-1).second : sat_->get(max_lb_i).second;

    static T zero = T();
    const T &min_v =
        (min_key <= sat_->get(min_lb_i).first && min_lb_i == 0) ? zero :
        (min_key <= sat_->get(min_lb_i).first) ? sat_->get(min_lb_i-1).second : sat_->get(min_lb_i).second;

    TRACE(min_v);
    TRACE(max_v);
    return max_v - min_v;
  }

  PersistentSAT<T> &operator+=(const PersistentSAT<T> &other) {
    if (other.sat_->size() == 0) {
      TRACE_BLOCK("they're empty");
      return *this;
    }
    if (sat_->size() == 0) {
      TRACE_BLOCK("we're empty");
      *this = other;
      return *this;
    }

    TRACE(*this); 
    TRACE(other);
    
    auto left_v = other.sat_->firstValue();

    add_mutate(left_v->value.first, left_v->value.second);

    TRACE(*this);

    // this is not very good, numerically speaking. But I don't
    // know how else I could do it.
    for (auto b = other.sat_->begin(1), e = other.sat_->end(); b!=e; ++b) {
      add_mutate((*b).first, (*b).second - (*left_v).value.second);
      TRACE(*this);
      left_v = b.value();
    }
    return *this;
  }
};

template <typename T>
PersistentSAT<T> operator+(const PersistentSAT<T> &v1,
                           const PersistentSAT<T> &v2)
{
  PersistentSAT<T> result(v1);
  result += v2;
  return result;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const PersistentSAT<T> &sat)
{
  os << "(";
  for (auto &pair: *sat.sat_) {
    os << "(" << pair.first << ", " << pair.second << "), ";
  }
  os << ")";
  return os;
}

/******************************************************************************/

template <typename T>
std::vector<T> drop_one(const std::vector<T> &v)
{
  std::vector<T> cp(v);
  cp.pop_back();
  return cp;
}

template <typename Summary>
struct SATGarbageCube
{
  BaseCube<PersistentSAT<Summary> > base_cube_;
  
  explicit SATGarbageCube(const std::vector<size_t> &widths):
      base_cube_(drop_one(widths)) {}

  void insert(const std::vector<size_t> &addresses, const Summary &summary) {
    // FIXME huh, this is annoyingly bad and annoyingly hard to fix nicely
    std::vector<size_t> first_addresses(drop_one(addresses));

    PersistentSAT<Summary> singleton;
    singleton.add_mutate(addresses.back(), summary);

    NCNodePointerType spine = base_cube_.insert_fresh_node(singleton, first_addresses, 0, 0);
    NCNodePointerType new_root = base_cube_.merge(0, spine, base_cube_.base_root_);

    base_cube_.make_node_ref(new_root, 0);
    base_cube_.release_node_ref(base_cube_.base_root_, 0);
    base_cube_.base_root_ = new_root;

    // at this point, it's likely that the base of spine will have no
    // references pointing to it.  since we won't hold any references,
    // we need to clean it up now.

    if (base_cube_.dims_[0].nodes.ref_counts[spine] == 0) {
      TRACE_BLOCK("clean spine");
      base_cube_.clean_node(spine, 0);
    }
  }

  template <typename SummaryPolicy>
  void range_query(
      SummaryPolicy &policy,
      const std::vector<std::pair<size_t, size_t> > &bounds) const;

  void print_dot(std::ostream &os, bool draw_garbage=false) {
    base_cube_.print_dot(os, draw_garbage);
  }

  void dump_nc(bool show_garbage=false) {
    base_cube_.dump_nc(show_garbage);
  }
};

template <typename Summary, typename SummaryPolicy>
struct SATSummaryPolicyAdaptor
{
  std::pair<size_t, size_t> last_bound_;
  const SATGarbageCube<Summary> &sat_cube_;
  SummaryPolicy &summary_policy_;
  
  SATSummaryPolicyAdaptor(
      const SATGarbageCube<Summary> &sat_cube,
      std::pair<size_t, size_t> last_bound,
      SummaryPolicy &summary_policy)
      : sat_cube_(sat_cube)
      , last_bound_(last_bound)
      , summary_policy_(summary_policy) {}

  void add(const PersistentSAT<Summary> &v) {
    Summary summ = v.sum(last_bound_.first, last_bound_.second);
    summary_policy_.add(summ);
  }
};

template <typename Summary>
template <typename SummaryPolicy>
void SATGarbageCube<Summary>::range_query(
    SummaryPolicy &policy,
    const std::vector<std::pair<size_t, size_t> > &bounds) const
{
  std::vector<std::pair<size_t, size_t> > garbage_bounds = drop_one(bounds);
  SATSummaryPolicyAdaptor<Summary, SummaryPolicy> adaptor(
      *this, bounds.back(), policy);
  base_cube_.range_query(adaptor, garbage_bounds);
}
    

/******************************************************************************/
// tests

bool test_persistentsat();
bool test_persistentsat_lower_bound();
bool test_persistentsat_sum();
bool test_persistentsat_addition();
bool test_naivecube_and_satgarbagecube_equivalence();

}

#endif
