#pragma once

namespace nc2 {

// This is a "naivecube" backed by an arrow table.
// There should be n total columns: n-1 of those columns have type UInt64 and correspond to addresses,
// and the nth column corresponds to the summary to be aggregated
//
// row-by-row insertion will be exceedingly slow;
// consider using "insert_table" instead of 
struct ArrowCube
{
  std::shared_ptr<arrow::Table> table_;
  std::vector<std::string> address_columns_;
  std::string summary_column_;

 public:

  explicit ArrowCube(std::shared_ptr<arrow::Table> table_,
                     const std::vector<std::string> address_columns_):
      table_(table_),
      address_columns_(address_columns_) {}

  void insert_table(std::shared_ptr<arrow::Table>);

  template <typename Summary, typename SummaryPolicy>
  void range_query(
      SummaryPolicy &policy,
      const std::vector<std::pair<uint64_t, uint64_t> > &bounds) const;
  
  template <typename Summary, typename SummaryPolicy>
  void range_query(
      SummaryPolicy &policy,
      const std::vector<std::pair<uint64_t, uint64_t> > &bounds,
      const std::vector<std::string> &other_column_names) const;
};

void ArrowCube::insert_table(std::shared_ptr<arrow::Table> new_table)
{
  // FIXME we should ensure that the schemata are compatible before
  // doing this right away

  table_ = arrow::ConcatenateTables({ table_, new_table }).ValueOrDie();
}

template <typename Summary, typename SummaryPolicy>
void ArrowCube::range_query(
    SummaryPolicy &policy,
    const std::vector<std::pair<uint64_t, uint64_t> > &bounds) const
{
  std::vector<std::string> other_column_names;
  range_query<Summary, SummaryPolicy>(policy, bounds, other_column_names);
}

template <typename Summary, typename SummaryPolicy>
void ArrowCube::range_query(
    SummaryPolicy &policy,
    const std::vector<std::pair<uint64_t, uint64_t> > &bounds,
    const std::vector<std::string> &other_column_names) const
{
  std::vector<ChunkedArrayIterator> itors;
  for (auto &col: address_columns_) {
    itors.push_back(ChunkedArrayIterator(table_->GetColumnByName(col)));
  }
  for (auto &other: other_column_names) {
    itors.push_back(ChunkedArrayIterator(table_->GetColumnByName(other)));
  }

  RowIterator rows(itors);
  while (!rows.next(true)) {
    bool in_bounds = true;
    for (size_t i = 0; i < bounds.size(); ++i) {
      const std::pair<uint64_t, uint64_t> &bound = bounds[i];
      if (rows.cols_[i].is_null()) {
        in_bounds = false;
        break;
      }
      uint64_t v = rows.cols_[i].value<arrow::UInt32Type>();
      if (v < bound.first || v >= bound.second) {
        in_bounds = false;
        break;
      }
    }
    if (in_bounds) {
      policy.add(rows);
    }
  }
}

};
