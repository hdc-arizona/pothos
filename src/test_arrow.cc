#include <arrow/ipc/feather.h>
#include <arrow/io/file.h>
#include <arrow/io/buffered.h>
#include <arrow/memory_pool.h>
#include <arrow/table.h>
#include <arrow/array.h>
#include <arrow/builder.h>

#include <boost/assert.hpp>

#include <iostream>
#include <memory>
#include <limits>
#include <iterator>
#include <algorithm>

#include "attribute_mapper.h"
#include "arrow_utils.h"

using namespace std;

void describe_table(shared_ptr<arrow::Table> arrow)
{
  auto schema = arrow->schema();

  cerr << "Description:" << endl;

  cerr << "  " << schema->num_fields() << " fields" << endl;
  for (auto &it: schema->field_names()) {
    cerr << "    " << it << endl;
  }
}

shared_ptr<arrow::Table> read_table(const std::string& path)
{
  arrow::Status st;

  auto file = arrow::io::ReadableFile::Open(path).ValueOrDie();

  std::shared_ptr<arrow::ipc::feather::Reader> feather_reader =
      arrow::ipc::feather::Reader::Open(file).ValueOrDie();

  shared_ptr<arrow::Table> arrow;

  st = feather_reader->Read(&arrow);
  if (!st.ok()) {
    cerr << "Error reading file" << endl;
    cerr << st.ToString() << endl;
  } else {
    cerr << "Read file " << path << " ok." << endl;
  }

  return arrow;
}


pair<double, double>
col_bounds(const std::string &colname,
           shared_ptr<arrow::Table> arrow)
{
  shared_ptr<arrow::ChunkedArray> col = arrow->GetColumnByName(colname);

  double min_so_far = numeric_limits<double>::max(),
      max_so_far = numeric_limits<double>::min();

  arrow_foreach<arrow::DoubleType>(
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
    shared_ptr<arrow::Table> arrow)
{
  shared_ptr<arrow::ChunkedArray> col = arrow->GetColumnByName(colname);

  size_t n = 0;
  double ex = 0.0, exx = 0.0;

  arrow_foreach<arrow::DoubleType>(
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
shared_ptr<arrow::Table>
convert_to_fixed(shared_ptr<arrow::Table> input,
                 vector<string> columns,
                 vector<T*> xforms
                 )
{
  vector<shared_ptr<arrow::Field> > fields;
  vector<shared_ptr<arrow::ChunkedArray> > arrays;
  
  for (size_t i = 0; i < columns.size(); ++i) {
    string &c = columns[i];
    fields.push_back(arrow::field(c, shared_ptr<arrow::DataType>(new arrow::UInt32Type), false));
    // shared_ptr<arrow::Array> array;
    shared_ptr<arrow::ChunkedArray> col = input->GetColumnByName(c);

    vector<shared_ptr<arrow::Array>> array_vector;
    auto chunks = col->chunks();
    for (auto &chunk: chunks) {
      arrow::NumericBuilder<arrow::UInt32Type> builder;
      auto float_chunk = std::static_pointer_cast<arrow::DoubleArray>(chunk);
      for (size_t j = 0; j < float_chunk->length(); ++j) {
        int32_t v = xforms[i]->convert(float_chunk->Value(j));
        OK_OR_DIE(builder.Append(v));
      }
      shared_ptr<arrow::Array> a;
      OK_OR_DIE(builder.Finish(&a));
      array_vector.push_back(a);
    }
    cerr << array_vector.size() << endl;
    cerr << col->chunks().size() << endl;
    shared_ptr<arrow::ChunkedArray> ca(new arrow::ChunkedArray(array_vector));
    arrays.push_back(ca);
    cerr << ca->length() << endl;
  }

  return arrow::Table::Make(
      arrow::schema(fields),
      arrays);
}

void test_with_nyc_pickup_data(std::string filename)
{
  shared_ptr<arrow::Table> arrow = read_table(filename);

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
  shared_ptr<arrow::Table> addresses =
      convert_to_fixed<CenterWidthAttribute>(
          arrow,
          { "pickup_latitude", "pickup_longitude" },
          xforms);

  // cerr << "Hello ?" << endl;
  // arrow_foreach<arrow::Int32Type>(
  //     addresses->GetColumnByName("pickup_longitude"),
  //     [](uint32_t v) {
  //       cerr << v << endl;
  //     });
  // cerr << "Hello ?" << endl;
  
  vector<size_t> counts_2d(resolution * resolution);
  size_t n = 0;
  arrow_foreach<arrow::Int32Type>(
      addresses->GetColumnByName("pickup_latitude"),
      addresses->GetColumnByName("pickup_longitude"),
      [&n, &counts_2d, &lat, &lon](uint32_t i_lat, uint32_t i_lon) {
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

int main(int argc, char **argv)
{
  if (argc < 2) {
    cerr << "Expected filename" << endl;
    exit(1);
  }

  test_with_nyc_pickup_data(argv[1]);
}
