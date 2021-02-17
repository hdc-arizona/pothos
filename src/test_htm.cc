#include "htm.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
  // Tri t = bounding_tri(Vec3(0.5, 0.5, 0.5), 0);
  cerr << htm_id(Vec3(0.5, 0.5, 0.5), 10) << endl;
}
