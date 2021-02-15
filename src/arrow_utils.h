#pragma once

#include <arrow/array.h>
#include <arrow/table.h>
#include <memory>
#include <unordered_map>

#include "arrow_macros.h"

/******************************************************************************/

template <typename TraitType, typename T>
void arrow_foreach(
    std::shared_ptr<arrow::ChunkedArray> column,
    T closure)
{
  for (auto &chunk: column->chunks()) {
    auto concrete_chunk = std::static_pointer_cast<typename arrow::TypeTraits<TraitType>::ArrayType>(chunk);
    for (int i = 0; i < concrete_chunk->length(); ++i) {
      closure(concrete_chunk->Value(i));
    }
  }
}

/******************************************************************************/

// this is lower overhead than the exec.h infra in arrow, but it's also
// clearly much less general. This assumes always same types and no nulls
// it's also not clear whether this is _faster_...
template <typename TraitType, typename T>
void arrow_foreach(
    std::shared_ptr<arrow::ChunkedArray> column_1,
    std::shared_ptr<arrow::ChunkedArray> column_2,
    T closure)
{
  auto chunks1 = column_1->chunks();
  auto chunks2 = column_2->chunks();

  size_t sz1 = chunks1.size(), sz2 = chunks2.size();

  size_t ci_1 = 0, ci_2 = 0;
  size_t i_1 = 0, i_2 = 0;
      
  if (ci_1 == sz1 || ci_2 == sz2) {
    // one of the two chunked arrays is empty: exit.
    return;
  }
  auto chunk_1 = std::static_pointer_cast<typename arrow::TypeTraits<TraitType>::ArrayType>(chunks1[ci_1]);
  auto chunk_2 = std::static_pointer_cast<typename arrow::TypeTraits<TraitType>::ArrayType>(chunks2[ci_2]);
  size_t sz_c1 = chunk_1->length(), sz_c2 = chunk_2->length();

  while (ci_1 < sz1 && ci_2 < sz2) {
    if (i_1 == sz_c1) {
      i_1 = 0;
      ++ci_1;
      chunk_1 = std::static_pointer_cast<typename arrow::TypeTraits<TraitType>::ArrayType>(chunks1[ci_1]);
      sz_c1 = chunk_1->length();
      continue; // this is slightly inefficient but clearer that we need to recheck bounds
    }
    if (i_2 == sz_c2) {
      i_2 = 0;
      ++ci_2;
      chunk_2 = std::static_pointer_cast<typename arrow::TypeTraits<TraitType>::ArrayType>(chunks2[ci_2]);
      sz_c2 = chunk_2->length();
      continue; // this is slightly inefficient but clearer that we need to recheck bounds
    }

    typename arrow::TypeTraits<TraitType>::CType
        v1 = chunk_1->Value(i_1),
        v2 = chunk_2->Value(i_2);

    closure(v1, v2);
    ++i_1;
    ++i_2;
  }
}

/******************************************************************************/

/// use this to access multiple values from a chunkedarray
struct ChunkedArrayAccessor
{
  std::vector<uint64_t> offset_bases;
  const arrow::ChunkedArray &chunked_array;
  size_t ix;

  // this assumes the the value `offset` implies a valid lookup into
  // the array
  
  // FIXME: figure out how to deal with null values
  template <typename Type>
  typename arrow::TypeTraits<Type>::CType value(size_t offset) {
    
    // if previously computed offset is valid, don't binary-search again.
    // hopefully this stays in L1 cache..
    if (!(offset_bases[ix] <= offset && offset_bases[ix+1] > offset)) {
      auto f = upper_bound(offset_bases.begin(), offset_bases.end(), offset) - 1;
      ix = f - offset_bases.begin();
    }

    // we might want to cache these as well. TODO: benchmark
    size_t diff = offset - offset_bases[ix];
    auto array_cast = std::static_pointer_cast<typename arrow::TypeTraits<Type>::ArrayType>(chunked_array.chunks()[ix]);
    return array_cast->Value(diff);
  }

  explicit ChunkedArrayAccessor(const arrow::ChunkedArray &chunked_array)
      : offset_bases()
      , chunked_array(chunked_array)
  {
    size_t sum_so_far = 0;
    for (size_t i = 0; i < chunked_array.chunks().size(); ++i) {
      offset_bases.push_back(sum_so_far);
      sum_so_far += chunked_array.chunks()[i]->length();
    }
    offset_bases.push_back(sum_so_far);
    ix = 0;
  }
};

/******************************************************************************/

struct ChunkedArrayIterator
{
  std::shared_ptr<arrow::ChunkedArray> array;
  std::shared_ptr<arrow::Array> chunk;
  size_t n_chunks;
  size_t chunk_ix;
  size_t chunk_offset;
  size_t array_offset;
    
  explicit ChunkedArrayIterator(std::shared_ptr<arrow::ChunkedArray> array):
      array(array),
      n_chunks(array->chunks().size()),
      chunk_ix(0),
      chunk_offset(0),
      array_offset(0)
  {
    if (n_chunks != 0) {
      chunk = array->chunks()[0];
    }
  }

  bool done() const {
    return chunk_ix == n_chunks;
  }

  size_t offset() const {
    return array_offset;
  }

  bool is_null() const {
    return chunk->IsNull(chunk_offset);
  }

  template <typename TraitType>
  typename arrow::TypeTraits<TraitType>::CType value() const
  {
    auto typed_chunk = std::static_pointer_cast<typename arrow::TypeTraits<TraitType>::ArrayType>(chunk);
    return typed_chunk->Value(chunk_offset);
  }

  void advance_chunk() {
    chunk_ix += 1;
    chunk_offset = 0;
    if (chunk_ix == n_chunks) {
      chunk = nullptr;
    } else {
      chunk = array->chunks()[chunk_ix];
    }
  }
  
  bool next(bool skip_null = true) {
    do {
      ++chunk_offset;
      ++array_offset;
      while (chunk_ix < n_chunks && chunk_offset >= chunk->length()) {
        advance_chunk();
      }
    } while (skip_null && !done() && is_null());
    
    return done();
  }

  /** skips a large number of entries in the iterator,
  and returns the array_offset at time the function exits.
  
  postcondition:
    array_offset >= min_array_offset
    if skip_null, then !is_null() or array_offset == array->size()
    (if array_offset == array->size(), the chunkedarray has ended)
  */
  size_t advance_until(size_t min_array_offset, bool skip_null = true) {
    if (done() || min_array_offset < array_offset) {
      return array_offset;
    }
    // array_advance indicates the total advance left
    size_t array_advance = min_array_offset - array_offset;
    size_t l = chunk->length();
    while (chunk_offset + array_advance >= l) {
      array_advance -= l - chunk_offset;
      array_offset += l - chunk_offset;
      advance_chunk();
      if (done()) {
        return array_offset;
      }
      l = chunk->length();
    }
    // here, chunk_offset + array_advance < l
    // which means we can advance chunk_offset by array_advance to get to min_array_offset
    // without spilling to the next chunk
    array_offset += array_advance;
    chunk_offset += array_advance;

    // now we're done, unless we were asked to skip nulls, in which case
    // we advance one by one until

    if (skip_null && !done() && is_null()) {
      next(true);
    }
    return array_offset;
  }
};

// A "row" is simply represented as a vector of column ChunkedArrayIterators
// We assume that all of these iterators have the same length()
struct RowIterator
{
  explicit RowIterator(const std::vector<ChunkedArrayIterator> &cols)
      : cols_(cols) {};

  std::vector<ChunkedArrayIterator> cols_;

  // when looking to skip nulls in next() calls, it's necessary to
  // call ensure_not_null() to initialize the row iterator to a
  // correct position.
  //
  // this is not included in the constructor because it's possible
  // there are no valid entries (all rows are null, ie for every index
  // at least one column is null)
  bool ensure_not_null() {
    bool need_skip = false;
    for (auto &it: cols_) {
      need_skip = need_skip || it.is_null();
    }
    if (need_skip) {
      return next(true);
    } else {
      return false;
    }
  }

  // advances all iterators in lockstep
  // postcondition:
  // cols_[i].array_offset == cols[j].array_offset for all i,j
  // 
  // if skip_null = true, then either all iterators are not null,
  // or all iterators are at the end of the array.
  bool next(bool skip_null = true) {
    // precondition: cols_[i].array_offset == cols_[j].array_offset for all i,j
    if (!skip_null) {
      bool result = true;
      for (auto &it: cols_) {
        result = result && it.next(false);
      }
      return result;
    }
    
    // precondition: skip_null == true
    cols_[0].next(true);
    do {
      size_t min_offset = cols_[0].array_offset;
      size_t max_other_offset = 0;
      for (size_t i = 0; i < cols_.size(); ++i) {
        max_other_offset = std::max(max_other_offset, cols_[i].advance_until(min_offset, true));
      }
      if (max_other_offset != min_offset) {
        cols_[0].advance_until(max_other_offset, true);
      } else {
        // if we arrive here, then max_other_offset = min_offset
        // 
        // either min_offset = length() in which case we need to break;
        // or min_offset < length(). since advance_until advances values until the output is
        // at least min_offset, if max_other_offset = min_offset, then all iterators stopped
        // at min_offset; and since min_offset < length() by assumption, this means all values
        // are non-null. This means all iterators are at the next entry for which all values
        // are identical, so we also break.
        break;
      }
    } while (1);

    // now, either all entries are not null or we're at the end of the array
    // we only need to check out one of them because at this point we know
    // the iterators are synchronized.
    return cols_[0].done();
  }
};

/******************************************************************************/
//
// assumes aggregation column is a double and addition is what we want.
//
// assumes address columns are all UInt32Type
//
// assumes arrays are non-empty, consistently chunked, and of the same size.
// 
// doesn't chunk arrays
//
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
  double aggregation_so_far = 0;
  std::vector<uint32_t> addresses(address_columns.size(), 0);
  
  // start by appending address (0, 0, 0, ..., 0) and aggregation 0 to the SAT
  for (size_t i = 0; i < address_columns.size(); ++i) {
    OK_OR_DIE(compressed_address_builders[i].Append(0));
  }
  OK_OR_DIE(compressed_aggregation_builder.Append(0));
  
  while (!row_itor.next()) {
    // first, increment integral
    aggregation_so_far += row_itor.cols_.back().value<arrow::DoubleType>();

    // then, check if address is now different. If so
    // add aggregation and current address
    bool differs = false;
    for (size_t i = 0; i < address_columns.size(); ++i) {
      uint32_t this_v = row_itor.cols_[i].value<arrow::UInt32Type>();
      if (this_v != addresses[i]) {
        differs = true;
      }
    }
    if (differs) {
      for (size_t i = 0; i < address_columns.size(); ++i) {
        addresses[i] = row_itor.cols_[i].value<arrow::UInt32Type>();
        OK_OR_DIE(compressed_address_builders[i].Append(addresses[i]));
      }
      OK_OR_DIE(compressed_aggregation_builder.Append(aggregation_so_far));
    }
  }

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

/******************************************************************************/
// reindex an unsorted chunked array given the result of sort_indices
// returns chunked array with the same chunking structure as the passed array.

template <typename ArrowType>
std::shared_ptr<arrow::ChunkedArray>
permute_chunked_array(
    const arrow::ChunkedArray &chunked_array,
    std::shared_ptr<arrow::Array> indices)
{
  // TODO: figure out how to do this type-generically.
  //
  // TODO: figure out how to handle nulls;
  // 
  // vector_sort.cc creates a vector of type with run-time-constructed types.
  // that's what we should base our impl off of.
  //
  // But I don't know how to make Builders work in this manner
  auto indices_cast = std::static_pointer_cast<arrow::UInt64Array>(indices);
  arrow::NumericBuilder<ArrowType> builder;

  ChunkedArrayAccessor accessor(chunked_array);

  std::vector<std::shared_ptr<arrow::Array>> vecs;

  size_t offset = 0;
  for (size_t i = 0; i < chunked_array.chunks().size(); ++i) {
    size_t chunk_size = chunked_array.chunks()[i]->length();
    for (size_t j = 0; j < chunk_size; ++j) {
      uint64_t ix = indices_cast->Value(offset++);
      uint32_t v = accessor.value<ArrowType>(ix);
      OK_OR_DIE(builder.Append(v));
    }
    vecs.push_back(builder.Finish().ValueOrDie());
  }

  return std::shared_ptr<arrow::ChunkedArray>(new arrow::ChunkedArray(vecs));
}
