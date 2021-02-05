#pragma once

#include <arrow/array.h>
#include <arrow/table.h>
#include <memory>

#define OK_OR_DIE(exp)                                          \
do {                                                            \
  if (!(exp).ok()) {                                            \
    cerr << "DIE AT " << __FILE__ << ":" << __LINE__ << endl;   \
    exit(1);                                                    \
  }                                                             \
} while (0);

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

// this is lower overhead than the exec.h infra in arrow, but it's also
// clearly much less general. This assumes always same types and no nulls
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
      chunk_offset(0)
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
    typedef typename arrow::TypeTraits<TraitType>::ArrayType A;
    A typed_chunk = std::static_pointer_cast<A>(chunk);
    return typed_chunk->Value(chunk_offset);
  }

  void advance_chunk() {
    chunk_ix += 1;
    chunk_offset = 0;
    chunk = array->chunks()[chunk_ix];
  }
  
  bool next(bool skip_null = true) {
    do {
      ++chunk_offset;
      ++array_offset;
      while (chunk_offset >= chunk->length() && chunk_ix < n_chunks) {
        advance_chunk();
      }
    } while (skip_null && (!done()) && is_null());
    
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
    if (min_array_offset < array_offset) {
      return is_null();
    }
    // array_advance indicates the total advance left
    size_t array_advance = min_array_offset - array_offset;
    size_t l = chunk->length();
    while (chunk_offset + array_advance >= l) {
      array_advance -= l - chunk_offset;
      array_offset += l - chunk_offset;
      advance_chunk();
      l = chunk->length();
    }
    // here, chunk_offset + array_advance < l
    // which means we can advance chunk_offset by array_advance to get to min_array_offset
    // without spilling to the next chunk
    array_offset += array_advance;
    chunk_offset += array_advance;

    // now we're done, unless we were asked to skip nulls, in which case
    // we advance one by one until

    if (skip_null && is_null()) {
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
