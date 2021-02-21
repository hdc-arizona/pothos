#include "arrow_utils.h"

#include <arrow/ipc/feather.h>
#include <arrow/io/file.h>
#include <arrow/io/buffered.h>

#include <iostream>
#include <algorithm>

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
    unordered_map<string, shared_ptr<ChunkedArray> > columns)
{
  vector<shared_ptr<Field>> fields;
  vector<shared_ptr<ChunkedArray>> arrays;

  for (auto &pair: columns) {
    fields.push_back(arrow::field(pair.first, pair.second->type(), false));
    arrays.push_back(pair.second);
  }
  return arrow::Table::Make(arrow::schema(fields), arrays);
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

template <typename AddressType,
          typename AggregationType>
std::shared_ptr<arrow::Table>
CompressAggregation<AddressType, AggregationType>::call(
    std::shared_ptr<arrow::Table> input,
    const std::vector<std::string> &address_columns,
    const std::string &aggregation_column)
{
  typedef typename TypeTraits<AddressType>::CType CAddressType;
  typedef typename TypeTraits<AggregationType>::CType CAggregationType;
  
  std::vector<ChunkedArrayIterator> itors;
  for (auto &name: address_columns) {
    itors.push_back(ChunkedArrayIterator(input->GetColumnByName(name)));
  }
  std::vector<arrow::NumericBuilder<AddressType>> compressed_address_builders(itors.size());
  
  itors.push_back(ChunkedArrayIterator(input->GetColumnByName(aggregation_column)));
  arrow::NumericBuilder<AggregationType> compressed_aggregation_builder;

  RowIterator row_itor(itors);
  DIE_WHEN(row_itor.done()); // this means no rows available.
    
  CAggregationType aggregation_so_far = 0;
  std::vector<CAddressType> addresses(address_columns.size(), 0);
  
  // start by appending address (0, 0, 0, ..., 0) and aggregation 0 to the SAT
  for (size_t i = 0; i < address_columns.size(); ++i) {
    OK_OR_DIE(compressed_address_builders[i].Append(0));
  }
  OK_OR_DIE(compressed_aggregation_builder.Append(0));
  
  do {
    // check if address is different
    bool differs = false;
    for (size_t i = 0; i < address_columns.size(); ++i) {
      CAddressType this_v = row_itor.cols_[i].value<AddressType>();
      if (this_v != addresses[i]) {
        differs = true;
      }
    }
    // If so, add old aggregation total using new address as boundary
    if (differs) {
      for (size_t i = 0; i < address_columns.size(); ++i) {
        addresses[i] = row_itor.cols_[i].value<AddressType>();
        OK_OR_DIE(compressed_address_builders[i].Append(addresses[i]));
      }
      OK_OR_DIE(compressed_aggregation_builder.Append(aggregation_so_far));
    }
    // last, increment aggregation
    aggregation_so_far += row_itor.cols_.back().value<AggregationType>();
  } while (!row_itor.next());

  // finally, add upper boundary of integral, address (max, max, ..., max) and
  // total aggregation to the SAT
  
  for (size_t i = 0; i < address_columns.size(); ++i) {
    OK_OR_DIE(compressed_address_builders[i].Append(
        std::numeric_limits<CAddressType>::max()));
  }
  OK_OR_DIE(compressed_aggregation_builder.Append(aggregation_so_far));

  std::vector<std::shared_ptr<arrow::Field> > fields;
  std::vector<std::shared_ptr<arrow::Array> > arrays;

  for (size_t i = 0; i < address_columns.size(); ++i) {
    fields.push_back(arrow::field(address_columns[i], std::shared_ptr<arrow::DataType>(new AddressType()), false));
    arrays.push_back(compressed_address_builders[i].Finish().ValueOrDie());
  }
  fields.push_back(arrow::field(aggregation_column, std::shared_ptr<arrow::DataType>(new AggregationType()), false));
  arrays.push_back(compressed_aggregation_builder.Finish().ValueOrDie());
  return arrow::Table::Make(
      arrow::schema(fields),
      arrays);
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
