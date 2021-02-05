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
