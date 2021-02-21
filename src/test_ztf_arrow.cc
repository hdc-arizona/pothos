#include <arrow/ipc/feather.h>
#include <arrow/io/file.h>
#include <arrow/io/buffered.h>
#include <arrow/memory_pool.h>
#include <arrow/table.h>
#include <arrow/array.h>
#include <arrow/builder.h>
#include <arrow/compute/api_vector.h>

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

/******************************************************************************/

using namespace std;
using namespace arrow;

int main(int argc, char **argv)
{
  shared_ptr<Table> t = read_feather_table("/Users/cscheid/data/ztf/feather-summaries/20200918_summary.feather_u");
  describe_table(t);

  {
    RowIterator rows({
        t->GetColumnByName("ra"),
        t->GetColumnByName("dec")
      });

    shared_ptr<ChunkedArray> htm_ids =
        map_rows<UInt64Type>(rows, [&rows]() {
          double ra = rows.cols_[0].value<DoubleType>(),
              dec = rows.cols_[1].value<DoubleType>();
          Vec2 v(ra, dec);
        
          uint64_t result = htm_id(to_sphere(v), 10);
          return result;
        });
    RowIterator rows_2({
        t->GetColumnByName("ra"),
        t->GetColumnByName("dec")
      });
    
    shared_ptr<ChunkedArray> one =
        map_rows<UInt64Type>(rows_2, []() {
          return 1;
        });

    t = arrow_cbind({
        t, make_table({
            {"htm_id", htm_ids},
            {"count", one}
          })});
  }

  compute::SortOptions options({
      compute::SortKey("htm_id", compute::SortOrder::Ascending)
    });

  auto sort_permutation = compute::CallFunction(
      "sort_indices", { Datum(t) }, &options).ValueOrDie().make_array();

  t = sort_table(t, &options);
  cerr << "Sorted" << endl;

  t = CompressAggregation<UInt64Type, UInt64Type>::call(t, { "htm_id" }, "count");
  cerr << "Aggregated" << endl;
    
  RowIterator({
      t->GetColumnByName("htm_id"),
      t->GetColumnByName("count")
    }).for_each([](RowIterator &rows) {
      cerr << rows.cols_[0].value<UInt64Type>() << " "
           << rows.cols_[1].value<UInt64Type>() << " "
           << endl;
   });
  return 0;
}
