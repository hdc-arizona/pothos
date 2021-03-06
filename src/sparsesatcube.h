#ifndef SPARSESATCUBE_H_
#define SPARSESATCUBE_H_

// #include <immutable/array.h>
#include <utility>
#include <iostream>
#include <boost/assert.hpp>
#include "sparsesat.h"
#include "debug.h"
#include "basecube.h"
#include "utils.h"

namespace nc2 {

/******************************************************************************/

template <typename Summary,
          template <typename> class Cube>
struct SparseSATCube
{
  Cube<SparseSAT<Summary> > base_cube_;

  explicit SparseSATCube(const std::vector<size_t> &widths):
      base_cube_(drop_one(widths)) {}

  void insert(const std::vector<size_t> &addresses, const Summary &summary) {
    SparseSAT<Summary> singleton;
    singleton.add_mutate(addresses.back(), summary);
    base_cube_.insert(addresses, singleton);
  }

  template <typename SummaryPolicy>
  void range_query(
      SummaryPolicy &policy,
      const std::vector<std::pair<size_t, size_t> > &bounds) const;

  void print_dot(std::ostream &os) {
    base_cube_.print_dot(os);
  }

  void dump_nc() {
    base_cube_.dump_nc();
  }

  void report_storage() {
    base_cube_.report_storage();
    size_t total_size = 0;
    for (size_t i=0; i<base_cube_.summaries_.size(); ++i) {
      total_size += base_cube_.summaries_[i].cum_array_.size();
    }
    std::cerr << "actual summaries storage cost:\n  " << total_size << std::endl;
  }
  
};

template <typename Summary,
          template <typename> class Cube,
          typename SummaryPolicy>
struct SparseSATSummaryPolicyAdaptor
{
  std::pair<size_t, size_t> last_bound_;
  const SparseSATCube<Summary, Cube> &sat_cube_;
  SummaryPolicy &summary_policy_;
  
  SparseSATSummaryPolicyAdaptor(
      const SparseSATCube<Summary, Cube> &sat_cube,
      std::pair<size_t, size_t> last_bound,
      SummaryPolicy &summary_policy)
      : last_bound_(last_bound)
      , sat_cube_(sat_cube)
      , summary_policy_(summary_policy) {}

  void add(const SparseSAT<Summary> &v) {
    Summary summ = v.sum(last_bound_.first, last_bound_.second);
    summary_policy_.add(summ);
  }
};

template <typename Summary,
          template <typename> class Cube>
template <typename SummaryPolicy>
void SparseSATCube<Summary, Cube>::range_query(
    SummaryPolicy &policy,
    const std::vector<std::pair<size_t, size_t> > &bounds) const
{
  std::vector<std::pair<size_t, size_t> > garbage_bounds = drop_one(bounds);
  SparseSATSummaryPolicyAdaptor<Summary, Cube, SummaryPolicy> adaptor(
      *this, bounds.back(), policy);
  base_cube_.range_query(adaptor, garbage_bounds);
}

/******************************************************************************/
// tests

bool test_naivecube_and_sparsesatgarbagecube_equivalence();
bool test_naivecube_and_sparsesatnanocube_equivalence();

};

#endif
