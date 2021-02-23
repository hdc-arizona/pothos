#include "htm.h"
#include <iostream>
#include <random>

using namespace std;
using namespace htm;

Vec3 random_point_on_sphere()
{
  Vec3 v;
  v.x = (rand() / (double) RAND_MAX) * 2 - 1;
  v.y = (rand() / (double) RAND_MAX) * 2 - 1;
  v.z = (rand() / (double) RAND_MAX) * 2 - 1;
  return v.normalize();
}

bool test_depth()
{
  size_t n = 100000;
  for (size_t i = 0; i < n; ++i) {
    Vec3 v = random_point_on_sphere();
    for (size_t j = 0; j < 21; ++j) {
      size_t h = htm_id(v, j);
      if (depth(h) != j) {
        cerr << "failed id " << h << " was expected to be level " << j
             << ", got " << depth(h) << " instead." << endl;
        return false;
      }
    }
  }
  cerr << "depth: Passed " << (n * 21) << " tests." << endl;
  return true;
}

bool test_ilog2()
{
  size_t n = 100000;
  for (size_t i = 0; i < n; ++i) {
    // generate a number w between 0 and 62
    size_t width = 63 * (rand() / (double) RAND_MAX);

    // generate a number q between 0.5 and 1.0
    //
    // ilog_2(int(q * 2^width)) = width - 1;
  
    double q = (rand() / (double) RAND_MAX) / 2.0 + 0.5;
    size_t test_number = (q * double(1LL << width));
  
    if (ilog2(test_number) != width-1) {
      cerr << "Failed (iteration " << i << ")! " << test_number << ", " << width << " expected " << width - 1
           << ", got " << ilog2(test_number) << endl;
      return false;
    }
  }
  cerr << "ilog2: passed " << n << " tests." << endl;

  for (size_t i = 0; i < 63; ++i) {
    if (ilog2(1LL << i) != i) {
      cerr << "Failed test for " << (1LL << i) << endl;
      return false;
    }
  }
  cerr << "ilog2: passed 63 tests." << endl;

  return true;
}

bool test_centroid()
{
  size_t n = 100000;
  for (size_t i = 0; i < n; ++i) {
    Vec3 v = random_point_on_sphere();
    for (size_t j = 0; j < 21; ++j) {
      size_t h = htm_id(v, j);
      Tri t = bounding_tri(v, j);
      Vec3 c = centroid(h);
      Vec3 oc = Vec3(0,0,0) - c;
      if (!t.contains(c)) {
        cerr << "Failed: centroid inconsistency " << h << ", " << j <<  " " << oc << endl;
        return false;
      }
      if (t.contains(oc)) {
        cerr << "Failed: centroid inconsistency " << h << ", " << j << " " << oc << endl;
        return false;
      }
    }
  }
  cerr << "centroid: passed " << (2 * n * 21) << " tests." << endl;
  
  return true;
}

bool test_htm()
{
  return test_ilog2() &&
      test_depth() &&
      test_centroid();
}

int main(int argc, char **argv)
{
  if (!test_htm())
    return 1;
}
