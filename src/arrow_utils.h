#pragma once

#include <arrow/compute/api_vector.h>
#include <arrow/io/file.h>
#include <arrow/io/api.h>
#include <arrow/builder.h>
#include <arrow/array.h>
#include <arrow/table.h>
#include <arrow/ipc/writer.h>

#include <memory>
#include <unordered_map>

#include "arrow_macros.h"

/******************************************************************************/

std::shared_ptr<arrow::Table> make_table(
    const std::unordered_map<std::string, std::shared_ptr<arrow::ChunkedArray>> &columns);

std::shared_ptr<arrow::Table> read_feather_table(const std::string &path);

void describe_table(std::shared_ptr<arrow::Table> arrow);

template <typename TraitType, typename T>
void arrow_foreach(
    std::shared_ptr<arrow::ChunkedArray> column,
    T closure);

template <typename TraitType, typename T>
void arrow_foreach(
    std::shared_ptr<arrow::ChunkedArray> column_1,
    std::shared_ptr<arrow::ChunkedArray> column_2,
    T closure);

void write_arrow(
    std::shared_ptr<arrow::Table> table,
    const std::string &path,
    int64_t max_chunk_size = -1);

/******************************************************************************/
/// cbind: combines columns from two different tables.
///
/// no error checking for now: tables are assumed to have the same
/// number of rows and

std::shared_ptr<arrow::Table>
arrow_cbind(const std::vector<std::shared_ptr<arrow::Table> > &tables);

/******************************************************************************/
/// CompressAggregation::call() takes a table sorted with respect to
/// address_columns and an aggregation column and compresses it such
/// that the tuple implied by address_columns rows is unique, and
/// aggregates over the aggregation column using the same name.
///
/// This simple data structure lies at the heart of a number of practical, fast
/// filtering tricks.
///
/// We're using static struct fields to do explicit template instantiation,
/// which I can't get the compiler to do for functions that change their
/// implementations based on templates, but do not change their signatures.
///
/// gotchas:
///
/// Assumes + is what we want
///
/// assumes arrays are non-empty, consistently chunked, and of the same size.
/// 
/// doesn't itself chunk the arrays
///

template <typename AddressType,
          typename AggregationType>
class CompressAggregation
{
 public:
  static std::shared_ptr<arrow::Table>
  call(
      std::shared_ptr<arrow::Table> input,
      const std::vector<std::string> &address_columns,
      const std::vector<std::string> &aggregation_column);
};

/******************************************************************************/
/// reindex an unsorted chunked array given the result of sort_indices
/// returns chunked array with the same chunking structure as the passed array.
///
/// gotchas:
/// 
/// assumes no nulls

template <typename ArrowType>
std::shared_ptr<arrow::ChunkedArray>
permute_chunked_array_t(
    const arrow::ChunkedArray &chunked_array,
    std::shared_ptr<arrow::Array> indices);

std::shared_ptr<arrow::ChunkedArray>
permute_chunked_array(
    const arrow::ChunkedArray &chunked_array,
    std::shared_ptr<arrow::Array> indices);

std::shared_ptr<arrow::Table>
permute_table(
    std::shared_ptr<arrow::Table> input,
    std::shared_ptr<arrow::Array> indices);

std::shared_ptr<arrow::Table>
sort_table(
    std::shared_ptr<arrow::Table> input,
    const arrow::compute::SortOptions *options);

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
//
// Do not reuse RowIterators!
struct RowIterator
{
  explicit RowIterator(std::shared_ptr<arrow::Table> table,
                       const std::vector<std::string> &names,
                       bool skip_null=true)
  {
    std::vector<std::shared_ptr<arrow::ChunkedArray>> cols;
    for (auto &name: names) {
      cols_.push_back(ChunkedArrayIterator(table->GetColumnByName(name)));
    }
    if (skip_null) {
      ensure_not_null();
    }
  }
  
  explicit RowIterator(const std::vector<ChunkedArrayIterator> &cols,
                       bool skip_null=true)
      : cols_(cols) {
    if (skip_null) {
      ensure_not_null();
    }
  };

  explicit RowIterator(const std::vector<std::shared_ptr<arrow::ChunkedArray>> &cols,
                       bool skip_null=true) {
    for (auto &col: cols) {
      cols_.push_back(ChunkedArrayIterator(col));
    }
    if (skip_null) {
      ensure_not_null();
    }
  }

  std::vector<ChunkedArrayIterator> cols_;

  bool some_null() const {
    bool some_null_b = false;
    for (auto &it: cols_) {
      some_null_b = some_null_b || it.is_null();
    }
    return some_null_b;
  }
  
  // when looking to skip nulls in next() calls, it's necessary to
  // call ensure_not_null() to initialize the row iterator to a
  // correct position.
  //
  // this is not included in the constructor because it's possible
  // there are no valid entries (all rows are null, ie for every index
  // at least one column is null)
  bool ensure_not_null() {
    if (some_null()) {
      return next(true);
    } else {
      return false;
    }
  }

  bool done() const {
    return cols_[0].done();
  }

  template <typename T>
  void for_each(T closure, bool skip_null = true) {
    do {
      closure(*this);
    } while (!next(skip_null));
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
        // the ordering here looks goofy so we don't get burned by the
        // shortcircuiting rules of &&
        result = it.next(false) && result;
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

// given a closure that expects a reference to the passed row iterator
// and which produces a value of TraitType::CType, construct
// a ChunkedArray of Array<TraitType> by iterating over the rows.
template <typename ArrowType, typename T>
std::shared_ptr<arrow::ChunkedArray>
map_rows(RowIterator &rows, T closure);

// same as above, but construct the RowIterator internally
// given a table and some columns
template <typename ArrowType, typename T>
std::shared_ptr<arrow::ChunkedArray>
map_rows(std::shared_ptr<arrow::Table> t,
         const std::vector<std::string> &col_names,
         T closure);

/******************************************************************************/
// create the table of sufficient statistics to fit gaussians.
// this is (|cols|+1)(|cols|+2)/2
//
// the selected columns must all be DoubleType

std::shared_ptr<arrow::Table>
make_gaussian_stats_table(
    std::shared_ptr<arrow::Table> t,
    const std::vector<std::string> &cols);

// This assumes all columns are to be selected.
std::shared_ptr<arrow::Table>
make_gaussian_stats_table(std::shared_ptr<arrow::Table> t);

/******************************************************************************/

#include "arrow_utils.hh"
