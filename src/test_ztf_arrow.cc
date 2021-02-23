#include <arrow/table.h>
#include <arrow/array.h>
#include <arrow/builder.h>
#include <arrow/compute/api_vector.h>
#include <arrow/pretty_print.h>

#include <boost/assert.hpp>

#include <iostream>
#include <memory>
#include <limits>
#include <iterator>
#include <algorithm>

#include "attribute_mapper.h"
#include "arrow_utils.h"
#include "arrow_convenience.h"
#include "arrowcube.h"

#include "htm.h"
#include <iomanip>
/******************************************************************************/

using namespace std;
using namespace arrow;
using namespace htm;

template <typename BuilderT>
std::shared_ptr<ChunkedArray> from_builder(BuilderT &b)
{
  return shared_ptr<ChunkedArray>(
      new ChunkedArray( { b.Finish().ValueOrDie() } ));
}

int main(int argc, char **argv)
{
  shared_ptr<Table> t = read_feather_table(
      "/Users/cscheid/data/ztf/feather-summaries/20200918_summary.feather_u");
  describe_table(t);

  {
    RowIterator rows({
        t->GetColumnByName("ra"),
        t->GetColumnByName("dec")
      });

    NumericBuilder<DoubleType> x, y, z;
    shared_ptr<ChunkedArray> htm_ids =
        map_rows<UInt64Type>(rows, [&rows, &x, &y, &z]() {
          double ra = rows.cols_[0].value<DoubleType>(),
              dec = rows.cols_[1].value<DoubleType>();
          Vec2 v(ra, dec);
          Vec3 c = to_cartesian(v);
          OK_OR_DIE(x.Append(c.x));
          OK_OR_DIE(y.Append(c.y));
          OK_OR_DIE(z.Append(c.z));
          uint64_t result = htm_id(c, 10);
          return result;
        });

    shared_ptr<Table> xyz = make_table({
        { "x", from_builder(x) },
        { "y", from_builder(y) },
        { "z", from_builder(z) }
      });

    shared_ptr<Table> g = make_gaussian_stats_table(xyz);

    t = arrow_cbind({t, g, make_table({{"htm_id", htm_ids}})});
  }

  compute::SortOptions options({
      compute::SortKey("htm_id", compute::SortOrder::Ascending)
    });
  t = sort_table(t, &options);
  t = CompressAggregation<UInt64Type, DoubleType>::call(
      t, { "htm_id" },
      { "magpsf",
        "s0",
        "s1", 
        "s2", 
        "s3", 
        "s4", 
        "s5", 
        "s6", 
        "s7", 
        "s8", 
        "s9" });
  cerr << "Aggregated" << endl;

  write_arrow(t, "agg.arrow");
  
  return 0;
}
