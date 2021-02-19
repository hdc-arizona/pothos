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
        
          uint64_t result = htm_id(to_sphere(v), 20);
          return result;
        });

    t = arrow_cbind({t, make_table({{"htm20", htm_ids}})});
  }

  compute::SortOptions options({
      compute::SortKey("htm20", compute::SortOrder::Ascending)
    });
  
  t = sort_table(t, &options);
    
  RowIterator({
      t->GetColumnByName("ra"),
      t->GetColumnByName("dec"),
      t->GetColumnByName("htm20")
    }).for_each([](RowIterator &rows) {
      cerr << rows.cols_[0].value<DoubleType>() << " "
           << rows.cols_[1].value<DoubleType>() << " "
           << rows.cols_[2].value<UInt64Type>() << " "
           << endl;
   });
  return 0;
}
