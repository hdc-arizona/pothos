#pragma once

#include <arrow/array.h>
#include <arrow/table.h>
#include <unordered_map>
#include <memory>

#include "arrow_macros.h"

// convenience functions for Apache Arrow
/******************************************************************************/

// convenience method to build a table out of named columns
// with no schema (so we use the column types to build one)
std::shared_ptr<arrow::Table> make_table(
    std::unordered_map<std::string, std::shared_ptr<arrow::ChunkedArray> > columns)
{
  std::vector<std::shared_ptr<arrow::Field>> fields;
  std::vector<std::shared_ptr<arrow::ChunkedArray>> arrays;

  for (auto &pair: columns) {
    fields.push_back(arrow::field(pair.first, pair.second->type(), false));
    arrays.push_back(pair.second);
  }
  return arrow::Table::Make(arrow::schema(fields), arrays);
}

// currently this only works for NumericBuilders, and it's pretty
// inefficient.
template <typename Type>
std::shared_ptr<arrow::ChunkedArray> make_chunked_array(
    const std::vector<typename arrow::TypeTraits<Type>::CType> &vec)
{
  arrow::NumericBuilder<Type> builder;
  for (auto &val: vec) {
    OK_OR_DIE(builder.Append(val));
  }
  return builder.Finish().ValueOrDie();
}
