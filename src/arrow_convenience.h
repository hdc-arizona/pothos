#pragma once

#include <arrow/array.h>
#include <arrow/table.h>
#include <memory>
#include <algorithm>

#include "arrow_macros.h"

// convenience functions for Apache Arrow
/******************************************************************************/

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
  return std::shared_ptr<arrow::ChunkedArray>(
      new arrow::ChunkedArray(
          std::vector<std::shared_ptr<arrow::Array>>({ builder.Finish().ValueOrDie() })));
}

template <typename Type>
std::shared_ptr<arrow::ChunkedArray> make_chunked_array(
    const std::vector<typename arrow::TypeTraits<Type>::CType> &vec,
    const std::vector<bool> &valid_entries)
{
  arrow::NumericBuilder<Type> builder;
  for (size_t i = 0; i < std::min(vec.size(), valid_entries.size()); ++i) {
    if (!valid_entries[i]) {
      OK_OR_DIE(builder.AppendNull());
    } else {
      OK_OR_DIE(builder.Append(vec[i]));
    }
  }
  return std::shared_ptr<arrow::ChunkedArray>(
      new arrow::ChunkedArray(
          std::vector<std::shared_ptr<arrow::Array>>({ builder.Finish().ValueOrDie() })));
}
