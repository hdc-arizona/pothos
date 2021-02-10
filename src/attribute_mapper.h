#pragma once

// transforms attributes to an address that can be used by the many *cubes

class DoubleAttribute
{
  double min, max;
  size_t resolution;

 public:
  
  DoubleAttribute(double min, double max, size_t resolution):
      min(min),
      max(max),
      resolution(resolution) {}

  size_t convert(double value) {
    if (value >= max) {
      return resolution-1;
    };
    if (value <= min) {
      return 0;
    }
    double t = (value - min) / (max - min);
    return size_t(t * resolution);
  }
};

class CenterWidthAttribute
{
  DoubleAttribute attr;

 public:

  CenterWidthAttribute(double center, double width, size_t resolution):
      attr(center-width/2.0, center+width/2.0, resolution) {}

  size_t convert(double value) {
    return attr.convert(value);
  }
};

// This performs one half of the bit interleaving operation.
// specifically, it takes a value with bits
//   hgfedcba
// to
//   0h0g0f0e0d0c0b0a
//
// To interleave two numbers, compute this to get two interleavings
// x and y, and then use x + (y << 1)
//
// This operates in 16 bit addresses.
//
// source: "Interleave bits by Binary Magic Numbers"
// https://graphics.stanford.edu/~seander/bithacks.html#InterleaveBMN

class BitInterleave
{
 public:
  
  size_t convert(size_t value) {
    static const unsigned int B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
    static const unsigned int S[] = {1, 2, 4, 8};

    unsigned int x = value & 65535;

    x = (x | (x << S[3])) & B[3];
    x = (x | (x << S[2])) & B[2];
    x = (x | (x << S[1])) & B[1];
    x = (x | (x << S[0])) & B[0];

    return x;
  }
};
