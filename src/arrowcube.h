#pragma once

namespace nc2 {

// This is a "naivecube" backed by an arrow table.
// There should be n total columns: n-1 of those columns have type UInt64 and correspond to addresses,
// and the nth column corresponds to the summary to be aggregated
//
// row-by-row insertion will be exceedingly slow;
// consider using "insert_table" instead of 
template <typename Summary>
struct ArrowCube
{
  std::shared_ptr<arrow::Table> table_;
  std::vector<std::string> address_columns_;
  std::string summary_column_;

 public:

  explicit ArrowCube(std::shared_ptr<arrow::Table> table_,
                     const std::vector<std::string> address_columns_,
                     std::string summary_column_):
      table_(table),
      address_columns_(address_columns_),
      summary_column_(summary_column_) {}

  void insert_table(std::shared_ptr<arrow::Table> table_);

  template <typename SummaryPolicy>
  Summary range_query(
      SummaryPolicy &policy,
      const std::vector<std::pair<size_t, size_t> > &bounds) const;
};

template <typename Summary>
void ArrowCube::insert_table(std::shared_ptr<arrow::Table> new_table)
{
  // FIXME we should ensure that the schemata are compatible before
  // doing this right away

  table_ = arrow::ConcatenateTables({ table_, new_table });
}

template <typename Summary, SummaryPolicy>
void range_query(
    SummaryPolicy &policy,
    const std::vector<std::pair<uint64_t, uint64_t> > &bounds) const;
{
  ChunkedArrayIterator summary_itor(table_->GetColumn(summary_column_));
  std::vector<ChunkedArrayIterator> itors;
  itors.push_back(summary_itor);

  for (auto &col: address_columns_) {
    ChunkedArrayIterator it(table_->GetColumn(col));
    itors.push_back(it);
  }

  RowIterator rows(itors);
  while (!next(true)) {
    policy.add(itors[0].value<typename CTypeTraits<Summary>>());
  }
}
