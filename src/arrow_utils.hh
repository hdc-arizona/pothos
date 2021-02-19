#pragma once

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

// this is simpler than the exec.h infra in arrow, but it's also
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

template <typename ArrowType>
std::shared_ptr<arrow::ChunkedArray>
permute_chunked_array_t(
    const arrow::ChunkedArray &chunked_array,
    std::shared_ptr<arrow::Array> indices)
{
  // TODO: figure out how to handle nulls;
  auto indices_cast = std::static_pointer_cast<arrow::UInt64Array>(indices);
  arrow::NumericBuilder<ArrowType> builder;

  ChunkedArrayAccessor accessor(chunked_array);

  std::vector<std::shared_ptr<arrow::Array>> vecs;

  size_t offset = 0;
  for (size_t i = 0; i < chunked_array.chunks().size(); ++i) {
    size_t chunk_size = chunked_array.chunks()[i]->length();
    for (size_t j = 0; j < chunk_size; ++j) {
      uint64_t ix = indices_cast->Value(offset++);
      typename arrow::TypeTraits<ArrowType>::CType v = accessor.value<ArrowType>(ix);
      OK_OR_DIE(builder.Append(v));
    }
    vecs.push_back(builder.Finish().ValueOrDie());
  }

  return std::shared_ptr<arrow::ChunkedArray>(new arrow::ChunkedArray(vecs));
}

// FIXME: this works for all numeric types, but not in general. Don't
// know how to do this in general.

/******************************************************************************/

template <typename ArrowType, typename T>
std::shared_ptr<arrow::ChunkedArray>
map_rows(RowIterator &rows, T closure)
{
  arrow::NumericBuilder<ArrowType> builder;
  do {
    if (rows.some_null()) {
      OK_OR_DIE(builder.AppendNull());
    } else {
      typename arrow::TypeTraits<ArrowType>::CType r = closure();
      OK_OR_DIE(builder.Append(r));
    }
  } while (!rows.next(false));
  std::cerr << "Done!" << std::endl;
  return std::shared_ptr<arrow::ChunkedArray>(new arrow::ChunkedArray({ builder.Finish().ValueOrDie() }));
}
