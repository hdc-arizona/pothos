#include "htm.h"
#include <iostream>
#include <random>

using namespace std;

int main(int argc, char **argv)
{
  // Tri t = bounding_tri(Vec3(0.5, 0.5, 0.5), 0);
  uint64_t h = 0;
  Vec3 v[16];
  for (size_t i = 0; i < 16; ++i) {
    v[i].x = (static_cast<double>(rand()) / static_cast<double>(RAND_MAX)) * 2.0 - 1.0;
    v[i].y = (static_cast<double>(rand()) / static_cast<double>(RAND_MAX)) * 2.0 - 1.0;
    v[i].z = (static_cast<double>(rand()) / static_cast<double>(RAND_MAX)) * 2.0 - 1.0;
    v[i] *= 1.0 / sqrtf(dot(v[i], v[i]));
    // cout << v[i] << endl;
  }

  size_t n = (1 << 24);
  for (size_t i = 0; i < n; ++i) {
    h = h + htm_id(v[i & 15], 21);
  }
  cout << h << endl;

  // Vec3 sdss_obj = to_sphere(Vec2(38.22259137, -8.47558474));
  // cout << "HTM16: " << htm_id(sdss_obj, 16) << endl;
  // cout << "HTM20: " << htm_id(sdss_obj, 20) << endl;
}
