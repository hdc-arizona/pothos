#pragma once

/******************************************************************************/
// a clean-room implementation of the HTM indexing code
//
// all we need is the computation of the htm_id for a given ra-dec, so
// this is significantly simpler than the full code.
//
// https://arxiv.org/pdf/cs/0701164.pdf

// FIXME this isn't an arrow thing.
#include "arrow_macros.h"
#include <iostream>

/******************************************************************************/

// sphere coords in ra/dec
struct Vec2
{
  double ra, dec;
  Vec2() {};
  Vec2(double ra, double dec): ra(ra), dec(dec) {}
  Vec2(const Vec2 &o): ra(o.ra), dec(o.dec) {}
  
  Vec2 &operator=(const Vec2 &o) {
    ra = o.ra;
    dec = o.dec;
    return *this;
  }
};

// cartesian sphere coords
struct Vec3
{
  double x, y, z;
  Vec3() {};
  Vec3(double x, double y, double z): x(x), y(y), z(z) {}
  Vec3(const Vec3 &o): x(o.x), y(o.y), z(o.z) {}

  inline Vec3 &operator=(const Vec3 &o) {
    x = o.x;
    y = o.y;
    z = o.z;
    return *this;
  }

  inline Vec3 &operator+=(const Vec3 &o) {
    x += o.x;
    y += o.y;
    z += o.z;
    return *this;
  }

  inline Vec3 &operator-=(const Vec3 &o) {
    x -= o.x;
    y -= o.y;
    z -= o.z;
    return *this;
  }
  
  inline Vec3& operator*=(double v) {
    x *= v;
    y *= v;
    z *= v;
    return *this;
  }

  inline Vec3& normalize() {
    double n = x * x + y * y + z * z;
    double s = 1.0 / sqrt(n);
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }
};

inline Vec3 operator+(const Vec3 &v1, const Vec3 &v2) {
  Vec3 result(v1);
  result += v2;
  return result;
}

inline Vec3 operator-(const Vec3 &v1, const Vec3 &v2) {
  Vec3 result(v1);
  result -= v2;
  return result;
}

inline Vec3 operator*(const Vec3 &v, double s) {
  Vec3 result(v);
  result *= s;
  return result;
}

inline Vec3 cross(const Vec3 &v1, const Vec3 &v2) {
  return Vec3(
      v1.y * v2.z - v2.y * v1.z,
      v1.z * v2.x - v2.z * v1.x,
      v1.x * v2.y - v2.x * v1.y);
}

inline double dot(const Vec3 &v1, const Vec3 &v2) {
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

std::ostream& operator<<(std::ostream &os, const Vec3 &v)
{
  return os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
}

struct Tri
{
  Vec3 v[3];
  
  Tri() {};
  Tri(const Vec3 &v1, const Vec3 &v2, const Vec3 &v3) {
    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
  }
  Tri(const Tri &o) {
    v[0] = o.v[0];
    v[1] = o.v[1];
    v[2] = o.v[2];
  }
  
  inline bool contains(const Vec3 &vec) const {
    const double eps = -1e-15;
    if (dot(vec, cross(v[0], v[1])) < eps) return false;
    if (dot(vec, cross(v[1], v[2])) < eps) return false;
    if (dot(vec, cross(v[2], v[0])) < eps) return false;
    return true;
  }
};

std::ostream& operator<<(std::ostream &os, const Tri &v)
{
  return os << "[" << v.v[0] << ", " << v.v[1] << ", " << v.v[2] << "]";
}

/******************************************************************************/

Vec3 to_sphere(const Vec2 &v)
{
  const double pi = 3.1415926535897932385;
  const double to_rad = pi / 180.0;

  double cd = cos(v.dec * to_rad);
  return Vec3(cos(v.ra  * to_rad) * cd,
              sin(v.ra  * to_rad) * cd,
              sin(v.dec * to_rad));
}

/******************************************************************************/

const Vec3 base_v[6] = {
  Vec3( 0.0,  0.0,  1.0),
  Vec3( 1.0,  0.0,  0.0),
  Vec3( 0.0,  1.0,  0.0),
  Vec3(-1.0,  0.0,  0.0),
  Vec3( 0.0, -1.0,  0.0),
  Vec3( 0.0,  0.0, -1.0)
};
const Tri base_t[8] = {
  Tri(base_v[1], base_v[5], base_v[2]), // S0
  Tri(base_v[2], base_v[5], base_v[3]), // S1
  Tri(base_v[3], base_v[5], base_v[4]), // S2
  Tri(base_v[4], base_v[5], base_v[1]), // S3
  Tri(base_v[1], base_v[0], base_v[4]), // N0
  Tri(base_v[4], base_v[0], base_v[3]), // N1
  Tri(base_v[3], base_v[0], base_v[2]), // N2
  Tri(base_v[2], base_v[0], base_v[1])  // N3
};

Tri bounding_tri(const Vec3 &v, size_t level)
{

  size_t i;
  for (i = 0; i < 8; ++i) {
    if (base_t[i].contains(v)) {
      break;
    }
  }
  // DIE_WHEN(i == 8);
  
  Tri t = base_t[i];
  while (level > 0) {
    // see the long comment below for a discussion on a faster variant
    // of the implementation
    //
    // const Vec3 w[3] = {
    //   (t.v[1] + t.v[2]) * 0.5,
    //   (t.v[0] + t.v[2]) * 0.5,
    //   (t.v[0] + t.v[1]) * 0.5
    // };
    Vec3 w[3] = {
      (t.v[1] + t.v[2]).normalize(),
      (t.v[0] + t.v[2]).normalize(),
      (t.v[0] + t.v[1]).normalize()
    };
    const Tri sector_t[4] = {
      Tri(t.v[0], w[2], w[1]),
      Tri(t.v[1], w[0], w[2]),
      Tri(t.v[2], w[1], w[0]),
      Tri(  w[0], w[1], w[2])
    };
    size_t i;
    for (i = 0; i < 4; ++i) {
      if (sector_t[i].contains(v)) {
        break;
      }
    }
    // DIE_WHEN(i == 4);
    t = sector_t[i];
    --level;
  }
  return t;
}

uint64_t htm_id(const Vec3 &v, size_t level)
{
  size_t i;
  for (i = 0; i < 8; ++i) {
    if (base_t[i].contains(v)) {
      break;
    }
  }

  // DIE_WHEN(i == 8);
  
  Tri t = base_t[i];
  size_t current_address = i + 8;
  while (level > 0) {
    /**************************************************************************/
    // Vec3 w[3] = {
    //   (t.v[1] + t.v[2]) * 0.5,
    //   (t.v[0] + t.v[2]) * 0.5,
    //   (t.v[0] + t.v[1]) * 0.5
    // };
    // If you're looking at that code and wondering why it's not
    // normalizing the vector, this is for you!
    // 
    // If we needed to store triangle vertices _on_ the surface of the
    // sphere, this would require us to normalize the lengths of the w
    // vectors. However, the triple scalar products computed by
    // Triangle::contains() are invariant to vector rescalings, (this
    // is obvious since they're fundamentally halfspace tests), and so
    // we don't need to normalize the vectors! This would provide
    // a real speedup.
    //
    // In addition, according to the HTM paper, at level 26, some of
    // the trig functions break down.  At that level, the product of
    // two 26-bit integers (represented in doubles) produces at most a
    // 52 bit mantissa.  So, for the range of HTM ids considered safe,
    // we should incur no precision loss at all in the cross product
    // of values in the triangle.
    //
    // However, when actually performing the halfspace test, we check
    // against eps=-1e-15. For this epsilon to be consistently
    // applied, we implicitly need all tested vectors to have the same
    // magnitude.
    // 
    // Without normalization, that's no longer the case by a factor of
    // up to 2^26 ~= 10^8. That's quite significant. So we split the
    // performance difference by performing an efficient operation,
    // namely dividing the vector by 2. The performance overhead of
    // this multiplication should be negligible, since it's just
    // decrementing the exponent of the number (which I expect to be
    // an optimization in the FPUs), and this operation keeps the
    // magnitude of the vectors bounded.
    //
    // According to
    // https://www.agner.org/optimize/instruction_tables.pdf,
    // a modern intel microarchitecture like Coffee Lake has the following timings:
    // 
    // fsqrt: 14-21 cycles
    // fdiv: 14-16 cycles
    // fmul: 5 cycles
    // 
    // Vec3 normalization takes three multiplications, one square
    // root, and three divisions. That's 61-74 cycles per triangle,
    // or about 200 cycles total, compared to 15 cycles (assuming the
    // FPU doesn't do better for multiplication by 0.5).
    //
    // So that's some ~180 cycles per level saved right there, about
    // 3800 cycles for computing a HTM id at the SDSS level. That's
    // almost a microsecond difference per HTM id computation.
    //
    // In a microbenchmark on my Coffee Lake MBP, I get about 2.75M
    // calls per second to compute HTM ids up to level 21 with this
    // trick, and 1.76M calls per second without. It's 50% faster!
    //
    // TODO: Write up why this is still a bad idea in the context
    // of the current calculations of HTM id
    /**************************************************************************/
    
    Vec3 w[3] = {
      (t.v[1] + t.v[2]).normalize(),
      (t.v[0] + t.v[2]).normalize(),
      (t.v[0] + t.v[1]).normalize()
    };
    // w[0] *= 1.0 / sqrt(dot(w[0], w[0]));
    // w[1] *= 1.0 / sqrt(dot(w[1], w[1]));
    // w[2] *= 1.0 / sqrt(dot(w[2], w[2]));
    
    const Tri sector_t[4] = {
      Tri(t.v[0], w[2], w[1]),
      Tri(t.v[1], w[0], w[2]),
      Tri(t.v[2], w[1], w[0]),
      Tri(  w[0], w[1], w[2])
    };
    size_t i;
    for (i = 0; i < 4; ++i) {
      if (sector_t[i].contains(v)) {
        break;
      }
    }
    // DIE_WHEN(i == 4);
    t = sector_t[i];
    current_address = current_address * 4 + i;
    --level;
  }
  return current_address;
}
