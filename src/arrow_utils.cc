#include "arrow_utils.h"

#include <arrow/ipc/feather.h>
#include <arrow/io/file.h>
#include <arrow/io/buffered.h>

#include <iostream>
#include <algorithm>
#include <sstream>
#include <memory>

#include "arrow_convenience.h"

using namespace std;
using namespace arrow;

/******************************************************************************/

void describe_table(std::shared_ptr<arrow::Table> arrow)
{
  auto schema = arrow->schema();

  cerr << "Description:" << endl;

  cerr << "  " << schema->num_fields() << " fields" << endl;
  cerr << "  " << arrow->column(0)->length() << " rows" << endl;
  for (auto &field: schema->fields()) {
    cerr << "    " << field->name() << ": " << field->type()->ToString() << endl;
  }
}

// convenience method to build a table out of named columns
// with no schema (so we use the column types to build one)
shared_ptr<Table> make_table(
    const unordered_map<string, shared_ptr<ChunkedArray> > &columns)
{
  vector<shared_ptr<Field>> fields;
  vector<shared_ptr<ChunkedArray>> arrays;

  for (auto &pair: columns) {
    fields.push_back(arrow::field(pair.first, pair.second->type(), false));
    arrays.push_back(pair.second);
  }
  return arrow::Table::Make(arrow::schema(fields), arrays);
}

void write_arrow(
    shared_ptr<Table> table,
    const string &path,
    int64_t max_chunk_size)
{
  shared_ptr<io::FileOutputStream>
      file = io::FileOutputStream::Open(path).ValueOrDie();

  shared_ptr<ipc::RecordBatchWriter>
      writer = ipc::MakeFileWriter(file, table->schema()).ValueOrDie();

  OK_OR_DIE(writer->WriteTable(*table, max_chunk_size));
  OK_OR_DIE(writer->Close());
}

/******************************************************************************/

shared_ptr<Table> read_feather_table(const std::string& path)
{
  Status st;

  auto file = io::ReadableFile::Open(path).ValueOrDie();

  std::shared_ptr<ipc::feather::Reader> feather_reader =
      ipc::feather::Reader::Open(file).ValueOrDie();

  shared_ptr<Table> arrow;

  st = feather_reader->Read(&arrow);
  if (!st.ok()) {
    cerr << "Error reading file" << endl;
    cerr << st.ToString() << endl;
  } else {
    cerr << "Read file " << path << " ok." << endl;
  }

  return arrow;
}

/******************************************************************************/

template <typename AddrT, typename AggrT>
std::shared_ptr<arrow::Table>
CompressAggregation<AddrT, AggrT>::call(
    std::shared_ptr<arrow::Table> input,
    const std::vector<std::string> &addr_columns,
    const std::vector<std::string> &aggr_columns)
{
  typedef typename TypeTraits<AddrT>::CType CAddrT;
  typedef typename TypeTraits<AggrT>::CType CAggrT;

  std::vector<ChunkedArrayIterator> itors;
  for (auto &name: addr_columns) {
    itors.push_back(ChunkedArrayIterator(input->GetColumnByName(name)));
  }
  std::vector<arrow::NumericBuilder<AddrT>> addr_builders(itors.size());

  size_t agg_cols_b = itors.size();
  for (auto &name: aggr_columns) {
    itors.push_back(ChunkedArrayIterator(input->GetColumnByName(name)));
  }
  size_t agg_cols_e = itors.size();
  
  std::vector<arrow::NumericBuilder<AggrT>>
      aggr_builders(aggr_columns.size());

  RowIterator row_itor(itors);
  DIE_WHEN(row_itor.done()); // this means no rows available.

  std::vector<CAggrT> aggr_so_far(aggr_columns.size(), CAggrT(0));
  std::vector<CAddrT> addresses(addr_columns.size(), CAddrT(0));
  
  // start by appending address (0, 0, 0, ..., 0) and aggregation (0,0,0,...,0) to the SAT
  for (size_t i = 0; i < addr_columns.size(); ++i) {
    OK_OR_DIE(addr_builders[i].Append(CAddrT(0)));
  }
  for (size_t i = 0; i < aggr_columns.size(); ++i) {
    OK_OR_DIE(aggr_builders[i].Append(CAggrT(0)));
  }
  
  do {
    // check if address is different
    bool differs = false;
    for (size_t i = 0; i < addr_columns.size(); ++i) {
      CAddrT this_v = row_itor.cols_[i].value<AddrT>();
      if (this_v != addresses[i]) {
        differs = true;
      }
    }
    // If so, add old aggregation total using new address as boundary
    if (differs) {
      for (size_t i = 0; i < addr_columns.size(); ++i) {
        addresses[i] = row_itor.cols_[i].value<AddrT>();
        OK_OR_DIE(addr_builders[i].Append(addresses[i]));
      }
      for (size_t i = 0; i < aggr_columns.size(); ++i) {
        OK_OR_DIE(aggr_builders[i].Append(aggr_so_far[i]));
      }
    }
    // last, increment aggregation
    for (size_t i = agg_cols_b; i < agg_cols_e; ++i) {
      aggr_so_far[i-agg_cols_b] += row_itor.cols_[i].value<AggrT>();
    }
  } while (!row_itor.next());

  // finally, add upper boundary of integral, address (max, max, ..., max) and
  // total aggregation to the SAT
  
  for (size_t i = 0; i < addr_columns.size(); ++i) {
    OK_OR_DIE(addr_builders[i].Append(std::numeric_limits<CAddrT>::max()));
  }
  for (size_t i = 0; i < aggr_columns.size(); ++i) {
    OK_OR_DIE(aggr_builders[i].Append(aggr_so_far[i]));
  }

  std::vector<std::shared_ptr<arrow::Field> > fields;
  std::vector<std::shared_ptr<arrow::Array> > arrays;
  std::unordered_map<std::string, std::shared_ptr<arrow::ChunkedArray>> cols;

  for (size_t i = 0; i < addr_columns.size(); ++i) {
    auto a = std::shared_ptr<arrow::ChunkedArray>(
        new ChunkedArray({ addr_builders[i].Finish().ValueOrDie() }));
    cols[addr_columns[i]] = a;
  }
  for (size_t i = 0; i < aggr_columns.size(); ++i) {
    auto a = std::shared_ptr<arrow::ChunkedArray>(
        new ChunkedArray({ aggr_builders[i].Finish().ValueOrDie() }));
    cols[aggr_columns[i]] = a;
  }
  return make_table(cols);
}

template class CompressAggregation<UInt32Type, UInt32Type>;
template class CompressAggregation<UInt32Type, UInt64Type>;
template class CompressAggregation<UInt32Type, FloatType>;
template class CompressAggregation<UInt32Type, DoubleType>;

template class CompressAggregation<UInt64Type, UInt32Type>;
template class CompressAggregation<UInt64Type, UInt64Type>;
template class CompressAggregation<UInt64Type, FloatType>;
template class CompressAggregation<UInt64Type, DoubleType>;

/******************************************************************************/

shared_ptr<Table>
arrow_cbind(const vector<shared_ptr<Table> > &tables)
{
  vector<shared_ptr<Field>> fields;
  vector<shared_ptr<ChunkedArray>> arrays;

  for (auto &table: tables) {
    vector<shared_ptr<Field>> table_fields = table->fields();
    vector<shared_ptr<ChunkedArray>> table_columns = table->columns();
    copy(table_fields.begin(), table_fields.end(), back_inserter(fields));
    copy(table_columns.begin(), table_columns.end(), back_inserter(arrays));
  }
  return Table::Make(arrow::schema(fields), arrays);
}

/******************************************************************************/
// this is ugly and makes a bloated binary. I don't know how to fix it, though..

// permutes an array generically. I'm sure this doesn't work
// for all types because there's an infinite number of them; but we
// should make it work for all numeric types.
std::shared_ptr<arrow::ChunkedArray>
permute_chunked_array(
    const arrow::ChunkedArray &chunked_array,
    std::shared_ptr<arrow::Array> indices)
{
  auto type = chunked_array.type();
  if (type->Equals(arrow::int8())) {
    return permute_chunked_array_t<Int8Type>(chunked_array, indices);
  } else if (type->Equals(arrow::uint8())) {
    return permute_chunked_array_t<UInt8Type>(chunked_array, indices);
  } else if (type->Equals(arrow::int16())) {
    return permute_chunked_array_t<Int16Type>(chunked_array, indices);
  } else if (type->Equals(arrow::uint16())) {
    return permute_chunked_array_t<UInt16Type>(chunked_array, indices);
  } else if (type->Equals(arrow::int32())) {
    return permute_chunked_array_t<Int32Type>(chunked_array, indices);
  } else if (type->Equals(arrow::uint32())) {
    return permute_chunked_array_t<UInt32Type>(chunked_array, indices);
  } else if (type->Equals(arrow::int64())) {
    return permute_chunked_array_t<Int64Type>(chunked_array, indices);
  } else if (type->Equals(arrow::uint64())) {
    return permute_chunked_array_t<UInt64Type>(chunked_array, indices);
  } else if (type->Equals(arrow::float32())) {
    return permute_chunked_array_t<FloatType>(chunked_array, indices);
  } else if (type->Equals(arrow::float64())) {
    return permute_chunked_array_t<DoubleType>(chunked_array, indices);
  } else {
    DIE("don't know how to handle type " << type->ToString());
  }
}

// assumes all columns are numeric...
std::shared_ptr<Table>
permute_table(std::shared_ptr<Table> input,
              std::shared_ptr<arrow::Array> indices)
{
  vector<shared_ptr<ChunkedArray> > sorted_columns;

  for (size_t i = 0; i < input->num_columns(); ++i) {
    sorted_columns.push_back(
        permute_chunked_array(
            *input->column(i),
            indices));
  }
  
  return Table::Make(input->schema(), sorted_columns); 
}

std::shared_ptr<Table>
sort_table(std::shared_ptr<Table> input,
           const arrow::compute::SortOptions *options)
{
  auto sort_permutation = compute::CallFunction(
      "sort_indices", { Datum(input) }, options).ValueOrDie().make_array();
  return permute_table(input, sort_permutation);
}

/******************************************************************************/

shared_ptr<Table>
make_gaussian_stats_table(
    const vector<shared_ptr<ChunkedArray>> &cols)
{
  vector<NumericBuilder<DoubleType>>
      builders((cols.size() + 1) * (cols.size() + 2) / 2);
  int sz = (int) cols.size();
  RowIterator(cols).for_each([&builders, &sz](RowIterator &rows) {
    size_t ix = 0;
    for (int i = -1; i < sz; ++i) {
      double v1 = (i == -1) ? 1.0 : rows.cols_[i].value<DoubleType>();
      for (int j = i; j < sz; ++j) {
        double v2 = (j == -1) ? 1.0 : rows.cols_[j].value<DoubleType>();
        OK_OR_DIE(builders[ix++].Append(v1 * v2));
      }
    }
  });
  unordered_map<string, shared_ptr<ChunkedArray>> result;
  size_t ix = 0;
  for (auto &builder: builders) {
    stringstream ss;
    ss << "s" << (ix++);
    result[ss.str()] = shared_ptr<ChunkedArray>(new ChunkedArray({ builder.Finish().ValueOrDie() }));
  }
  return make_table(result);
}

shared_ptr<Table>
make_gaussian_stats_table(
    shared_ptr<Table> t,
    const vector<string> &col_names)
{
  vector<shared_ptr<ChunkedArray>>
      cols;
  for (auto &name: col_names) {
    cols.push_back(t->GetColumnByName(name));
  }
  return make_gaussian_stats_table(cols);
}

shared_ptr<Table>
make_gaussian_stats_table(shared_ptr<Table> t)
{
  return make_gaussian_stats_table(t->columns());
}

std::shared_ptr<Table> arrow_select_columns(
    shared_ptr<Table> t,
    const std::vector<string> &names)
{
  std::vector<int> indices;
  auto schema = t->schema();
  for (const auto& name: names) {
    int ix = schema->GetFieldIndex(name);
    if (ix == -1) {
      return nullptr;
    }
    indices.push_back(ix);
  }
  return t->SelectColumns(indices).ValueOrDie();
}
