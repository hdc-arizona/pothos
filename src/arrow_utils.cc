#include "arrow_utils.h"

#include <arrow/ipc/feather.h>
#include <arrow/io/file.h>
#include <arrow/io/buffered.h>

#include <iostream>

using namespace std;
using namespace arrow;

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

std::shared_ptr<arrow::Table>
compress_aggregation(
    std::shared_ptr<arrow::Table> input,
    const std::vector<std::string> &address_columns,
    const std::string &aggregation_column)
{
  std::vector<ChunkedArrayIterator> itors;
  for (auto &name: address_columns) {
    itors.push_back(ChunkedArrayIterator(input->GetColumnByName(name)));
  }
  std::vector<arrow::NumericBuilder<arrow::UInt32Type>> compressed_address_builders(itors.size());
  
  itors.push_back(ChunkedArrayIterator(input->GetColumnByName(aggregation_column)));
  arrow::NumericBuilder<arrow::DoubleType> compressed_aggregation_builder;

  RowIterator row_itor(itors);
  DIE_WHEN(row_itor.done()); // this means no rows available.
    
  double aggregation_so_far = 0;
  std::vector<uint32_t> addresses(address_columns.size(), 0);
  
  // start by appending address (0, 0, 0, ..., 0) and aggregation 0 to the SAT
  for (size_t i = 0; i < address_columns.size(); ++i) {
    OK_OR_DIE(compressed_address_builders[i].Append(0));
  }
  OK_OR_DIE(compressed_aggregation_builder.Append(0));
  
  do {
    // check if address is different
    bool differs = false;
    for (size_t i = 0; i < address_columns.size(); ++i) {
      uint32_t this_v = row_itor.cols_[i].value<arrow::UInt32Type>();
      if (this_v != addresses[i]) {
        differs = true;
      }
    }
    // If so, add old aggregation total using new address as boundary
    if (differs) {
      for (size_t i = 0; i < address_columns.size(); ++i) {
        addresses[i] = row_itor.cols_[i].value<arrow::UInt32Type>();
        OK_OR_DIE(compressed_address_builders[i].Append(addresses[i]));
      }
      OK_OR_DIE(compressed_aggregation_builder.Append(aggregation_so_far));
    }
    // last, increment aggregation
    aggregation_so_far += row_itor.cols_.back().value<arrow::DoubleType>();
  } while (!row_itor.next());

  // finally, add upper boundary of integral, address (max, max, ..., max) and
  // total aggregation to the SAT
  
  for (size_t i = 0; i < address_columns.size(); ++i) {
    OK_OR_DIE(compressed_address_builders[i].Append(std::numeric_limits<uint32_t>::max()));
  }
  OK_OR_DIE(compressed_aggregation_builder.Append(aggregation_so_far));

  std::vector<std::shared_ptr<arrow::Field> > fields;
  std::vector<std::shared_ptr<arrow::Array> > arrays;

  for (size_t i = 0; i < address_columns.size(); ++i) {
    fields.push_back(arrow::field(address_columns[i], arrow::uint32(), false));
    arrays.push_back(compressed_address_builders[i].Finish().ValueOrDie());
  }
  fields.push_back(arrow::field(aggregation_column, arrow::float64(), false));
  arrays.push_back(compressed_aggregation_builder.Finish().ValueOrDie());
  return arrow::Table::Make(
      arrow::schema(fields),
      arrays);
}
