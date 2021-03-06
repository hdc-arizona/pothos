#include <arrow/memory_pool.h>
#include <arrow/table.h>
#include <arrow/array.h>
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

/******************************************************************************/

using namespace std;
using namespace arrow;

pair<double, double>
col_bounds(const std::string &colname,
           shared_ptr<Table> arrow)
{
  shared_ptr<ChunkedArray> col = arrow->GetColumnByName(colname);

  double min_so_far = numeric_limits<double>::max(),
      max_so_far = numeric_limits<double>::min();

  arrow_foreach<DoubleType>(
      col,
      [&min_so_far,
       &max_so_far](double v) {
        min_so_far = std::min(min_so_far, v);
        max_so_far = std::max(max_so_far, v);
      });
  
  return make_pair(min_so_far, max_so_far);
}

pair<double, double>
col_mean_stdev(
    const std::string &colname,
    shared_ptr<Table> arrow)
{
  shared_ptr<ChunkedArray> col = arrow->GetColumnByName(colname);

  size_t n = 0;
  double ex = 0.0, exx = 0.0;

  arrow_foreach<DoubleType>(
      col,
      [&n, &ex, &exx](double v) {
        // drop nans
        if (v != v)
          return;
        ++n;
        ex += v;
        exx += v * v;
      });
  ex /= n;
  exx /= n;
  return make_pair(ex, sqrt(exx - ex * ex)); // this is numerically unstable, :shrug:
}

ostream &operator<<(ostream &os, const std::pair<double, double> &v)
{
  return os << "(" << v.first << "," << v.second << ")";
}

template <typename T1, typename T2>
void write_ppm(
    size_t width, size_t height,
    ostream &os,
    const std::vector<T2> &vec,
    T1 closure)
{
  os << "P3\n" << width << " " << height << "\n255\n";
  for (size_t y = 0; y < height; ++y) {
    for (size_t x = 0; x < width; ++x) {
      const T2 &v = vec[y * width + x];
      int r, g, b;
      std::tie(r, g, b) = closure(v);
      os << r << " " << g << " " << b << endl;
    }
  }
}

std::tuple<int, int, int> color(float u)
{
  if (u == 0) {
    return make_tuple(0, 0, 0);
  }
  u *= 3;
  if (u < 1) {
    return make_tuple(int(u * 255), 0, 0);
  }
  if (u < 2) {
    u -= 1;
    return make_tuple(255, int(u * 255), 0);
  }
  u -= 2;
  return make_tuple(255, 255, int(u * 255));
}

template <typename T>
// This should be done more carefully for large data.
shared_ptr<Table>
convert_to_fixed(shared_ptr<Table> input,
                 vector<string> columns,
                 vector<T*> xforms
                 )
{
  vector<shared_ptr<Field> > fields;
  vector<shared_ptr<ChunkedArray> > arrays;
  
  for (size_t i = 0; i < columns.size(); ++i) {
    string &c = columns[i];
    fields.push_back(field(c, shared_ptr<DataType>(new UInt32Type), false));
    // shared_ptr<Array> array;
    shared_ptr<ChunkedArray> col = input->GetColumnByName(c);

    vector<shared_ptr<Array>> array_vector;
    auto chunks = col->chunks();
    for (auto &chunk: chunks) {
      NumericBuilder<UInt32Type> builder;
      auto float_chunk = std::static_pointer_cast<DoubleArray>(chunk);
      for (size_t j = 0; j < float_chunk->length(); ++j) {
        int32_t v = xforms[i]->convert(float_chunk->Value(j));
        OK_OR_DIE(builder.Append(v));
      }
      shared_ptr<Array> a;
      OK_OR_DIE(builder.Finish(&a));
      array_vector.push_back(a);
    }
    cerr << array_vector.size() << endl;
    cerr << col->chunks().size() << endl;
    shared_ptr<ChunkedArray> ca(new ChunkedArray(array_vector));
    arrays.push_back(ca);
    cerr << ca->length() << endl;
  }

  return Table::Make(
      schema(fields),
      arrays);
}

shared_ptr<Table> make_address_table(shared_ptr<Table> arrow)
{
  cerr << "Bounds:" << endl;
  cerr << "  pickup_latitude: " << col_bounds("pickup_latitude", arrow) << endl;
  cerr << "  pickup_longitude: " << col_bounds("pickup_longitude", arrow) << endl;
  cerr << "  dropoff_latitude: " << col_bounds("dropoff_latitude", arrow) << endl;
  cerr << "  dropoff_longitude: " << col_bounds("dropoff_longitude", arrow) << endl;
  cerr << "mean/stdev:" << endl;
  cerr << "  pickup_latitude: " << col_mean_stdev("pickup_latitude", arrow) << endl;
  cerr << "  pickup_longitude: " << col_mean_stdev("pickup_longitude", arrow) << endl;
  cerr << "  dropoff_latitude: " << col_mean_stdev("dropoff_latitude", arrow) << endl;
  cerr << "  dropoff_longitude: " << col_mean_stdev("dropoff_longitude", arrow) << endl;

  const size_t resolution = 256;
  
  // DoubleAttribute lat( 40.5,  41, resolution);
  // DoubleAttribute lon(-74.25, -73.75, resolution);
  CenterWidthAttribute lat(40.75, 0.25, resolution);
  CenterWidthAttribute lon(-74, 0.25, resolution);

  vector<CenterWidthAttribute*> xforms = {&lat, &lon};
  shared_ptr<Table> addresses =
      convert_to_fixed<CenterWidthAttribute>(
          arrow,
          { "pickup_latitude", "pickup_longitude" },
          xforms);
  
  return addresses;
}

void test_with_nyc_pickup_data(std::string filename)
{
  shared_ptr<Table> arrow = read_feather_table(filename);

  // cerr << "Bounds:" << endl;
  // cerr << "  pickup_latitude: " << col_bounds("pickup_latitude", arrow) << endl;
  // cerr << "  pickup_longitude: " << col_bounds("pickup_longitude", arrow) << endl;
  // cerr << "  dropoff_latitude: " << col_bounds("dropoff_latitude", arrow) << endl;
  // cerr << "  dropoff_longitude: " << col_bounds("dropoff_longitude", arrow) << endl;
  // cerr << "mean/stdev:" << endl;
  // cerr << "  pickup_latitude: " << col_mean_stdev("pickup_latitude", arrow) << endl;
  // cerr << "  pickup_longitude: " << col_mean_stdev("pickup_longitude", arrow) << endl;
  // cerr << "  dropoff_latitude: " << col_mean_stdev("dropoff_latitude", arrow) << endl;
  // cerr << "  dropoff_longitude: " << col_mean_stdev("dropoff_longitude", arrow) << endl;

  const size_t resolution = 256;
  
  // // DoubleAttribute lat( 40.5,  41, resolution);
  // // DoubleAttribute lon(-74.25, -73.75, resolution);
  // CenterWidthAttribute lat(40.75, 0.25, resolution);
  // CenterWidthAttribute lon(-74, 0.25, resolution);

  // vector<CenterWidthAttribute*> xforms = {&lat, &lon};
  shared_ptr<Table> addresses = make_address_table(arrow);
      // convert_to_fixed<CenterWidthAttribute>(
      //     arrow,
      //     { "pickup_latitude", "pickup_longitude" },
      //     xforms);

  // cerr << "Hello ?" << endl;
  // arrow_foreach<Int32Type>(
  //     addresses->GetColumnByName("pickup_longitude"),
  //     [](uint32_t v) {
  //       cerr << v << endl;
  //     });
  // cerr << "Hello ?" << endl;
  
  vector<size_t> counts_2d(resolution * resolution);
  size_t n = 0;
  arrow_foreach<Int32Type>(
      addresses->GetColumnByName("pickup_latitude"),
      addresses->GetColumnByName("pickup_longitude"),
      [&n, &counts_2d](uint32_t i_lat, uint32_t i_lon) {
        ++n;
        i_lat = resolution - 1 - i_lat;
        counts_2d[i_lon + resolution * i_lat]++;
      });
  size_t mx = *std::max_element(counts_2d.begin(), counts_2d.end());
  cerr << n << " " << mx << endl;

  write_ppm(resolution, resolution,
            cout,
            counts_2d,
            [&mx](size_t v) {
              float u = log(v + 1) / log(mx + 1);
              return color(u);
            });
}

struct CountPolicy
{
  int count;
  CountPolicy(): count(0) {};
  
  inline void add(const RowIterator &row) {
    ++count;
  }
};

/******************************************************************************/

void test_arrow_cube(std::string filename)
{
  shared_ptr<Table>
      arrow = read_feather_table(filename),
      addresses = make_address_table(arrow);
      
  // nc2::ArrowCube ac(addresses, { "pickup_latitude", "pickup_longitude" });
  // CountPolicy policy;
  // ac.range_query<size_t, CountPolicy>(policy, { make_pair(size_t(0), size_t(256)), make_pair(size_t(0), size_t(256)) });
  // cerr << policy.count << endl;

  // compute::SortOrder order = compute::SortOrder::Ascending;
  compute::SortOptions options({
      compute::SortKey("pickup_latitude", compute::SortOrder::Ascending)
    });
  auto sorted_df = sort_table(addresses, &options);
}

void test_arrow_convenience()
{
  shared_ptr table = make_table(
      {{ "col1", make_chunked_array<UInt32Type>({ 0, 0, 0, 1, 1, 1, 2, 2, 2 }) },
       { "col2", make_chunked_array<UInt32Type>({ 0, 0, 1, 0, 2, 2, 1, 2, 2 }) },
       { "agg",  make_chunked_array<DoubleType>({ 1, 1, 1, 1, 1, 1, 1, 1, 1 }) }
      });

  {
    RowIterator itor({
        ChunkedArrayIterator(table->GetColumnByName("col1")),
        ChunkedArrayIterator(table->GetColumnByName("col2")),
        ChunkedArrayIterator(table->GetColumnByName("agg"))
      });
    
    do {
      for (size_t i = 0; i < 2; ++i) {
        cerr << itor.cols_[i].value<UInt32Type>() << " ";
      }
      cerr << "agg: " << itor.cols_[2].value<DoubleType>() << endl;
    } while (!itor.next());
  }
  
  table = CompressAggregation<UInt32Type, DoubleType>::call(
      table, { "col1", "col2" }, { "agg" });

  {
    RowIterator itor({
        ChunkedArrayIterator(table->GetColumnByName("col1")),
        ChunkedArrayIterator(table->GetColumnByName("col2")),
        ChunkedArrayIterator(table->GetColumnByName("agg"))
      });

    do {
      for (size_t i = 0; i < 2; ++i) {
        cerr << itor.cols_[i].value<UInt32Type>() << " ";
      }
      cerr << "agg: " << itor.cols_[2].value<DoubleType>() << endl;
    } while (!itor.next());
  }
}


int main(int argc, char **argv)
{
  const std::string filename = "/Users/cscheid/data/nyc-tlc/feather/yellow_tripdata_2014-02.feather";
  // test_with_nyc_pickup_data(argv[1]);
  // test_arrow_cube(filename);
  test_arrow_convenience();
}
