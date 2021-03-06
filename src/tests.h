#ifndef TESTS_H_
#define TESTS_H_

#include "naivecube.h"
#include "utils.h"
#include <iostream>
#include <fstream>

#define RUN_TEST(t) if (!t) { std::cerr << "test " << #t << " failed." << std::endl; exit(1); } else { std::cerr << "test " << #t << " passed." << std::endl; }

#define CHECK_THAT(exp) if (!(exp)) {                             \
  std::cerr << "check " << #exp << " failed." << std::endl;       \
  return false;                                                   \
}

template <typename Summary,
          template <typename> class Cube1,
          template <typename> class Cube2>
bool test_query_equivalence(
    const Cube1<Summary> &c1,
    const Cube2<Summary> &c2,
    const std::vector<std::pair<size_t, size_t> > &range)
{
  nc2::CombineSummaryPolicy<int> p1, p2;
  c1.range_query(p1, range);
  c2.range_query(p2, range);

  if (p1.total != p2.total) {
    std::cerr << "Failure! with query (";
    for (size_t k=0; k<c1.dims_.size(); ++k) {
      std::cerr << "(" << range[k].first << "," << range[k].second << "),";
    }
    std::cerr << ")\n";
        
    std::cerr << "data structures disagree: cube1 " << p1.total << " vs cube2 " << p2.total << std::endl;

    return false;
  }
  return true;
}

template <typename Summary,
          template <typename> class Cube1,
          template <typename> class Cube2>
bool test_randomized_query_equivalence(
    const Cube1<Summary> &c1,
    const Cube2<Summary> &c2,
    int n_queries)
{
  std::vector<size_t> widths;
  for (auto &d: c1.dims_) {
    widths.push_back(d.width);
  }
  for (size_t j=0; j<n_queries; ++j) {
    std::vector<std::pair<size_t, size_t> > range;
    size_t n_dims = widths.size();
    for (size_t k=0; k<n_dims; ++k) {
      size_t v1 = rand() % (1 << widths[k]),
             v2 = rand() % (1 << widths[k]);
      range.push_back(std::make_pair(std::min(v1, v2), std::max(v1, v2)));
    }

    if (!test_query_equivalence<Summary, Cube1, Cube2>(c1, c2, range))
      return false;
  }
  return true;
}


template <template <typename> class Cube>
bool test_equivalence_to_naivecube(
    const std::vector<size_t> &widths,
    const std::vector<std::vector<size_t> > &points,
    const std::vector<std::pair<size_t, size_t> > &range)
{
  nc2::NaiveCube<int> naivecube(widths);
  Cube<int> cube(widths);
  for (auto const &e: points) {
    naivecube.insert(e, 1);
    cube.insert(e, 1);
    WHEN_TRACING {
      cube.dump_nc(true);
    }
    {
      std::ofstream out("nc.dot");
      cube.print_dot(out);
    }
  }
  
  nc2::CombineSummaryPolicy<int> p1, p2;
  naivecube.range_query(p1, range);
  cube.range_query(p2, range);

  if (p1.total != p2.total) {
    std::cerr << "Failure!\n";
    std::cerr << "data structures disagree: naivecube " << p1.total << " vs cube " << p2.total << std::endl;
    return false;
  }
  return true;
}

template <template <typename> class Cube>
bool test_randomized_equivalence_to_naivecube(
    const std::vector<size_t> &widths,
    size_t n_runs,
    size_t n_points,
    size_t n_queries)
{
  size_t width = widths.size();
  for (size_t i=0; i<n_runs; ++i) {
    nc2::NaiveCube<int> naivecube(widths);
    Cube<int> cube(widths);
    std::vector<size_t> points;

    for (size_t j=0; j<n_points; ++j) {
      std::vector<size_t> address;
      for (size_t k=0; k<width; ++k) {
        size_t v = rand() % (1 << widths[k]);
        address.push_back(v);
        points.push_back(v);
      }
      naivecube.insert(address, 1);
      cube.insert(address, 1);
    }

    for (size_t j=0; j<n_queries; ++j) {
      std::vector<std::pair<size_t, size_t> > range;
      for (size_t k=0; k<width; ++k) {
        size_t v1 = rand() % (1 << widths[k]),
               v2 = rand() % (1 << widths[k]);
        range.push_back(std::make_pair(std::min(v1, v2), std::max(v1, v2)));
      }

      nc2::CombineSummaryPolicy<int> p1, p2;
      naivecube.range_query(p1, range);
      cube.range_query(p2, range);

      if (p1.total != p2.total) {
        std::cerr << "Failure! On dataset:" << std::endl;
        copy(points.begin(), points.end(),
             std::ostream_iterator<size_t>(std::cerr, " "));
        std::cerr << std::endl;
        std::cerr << "with query (";
        for (size_t k=0; k<width; ++k) {
          std::cerr << "(" << range[k].first << "," << range[k].second << "),";
        }
        std::cerr << ")\n";
        
        std::cerr << "data structures disagree: naivecube " << p1.total << " vs cube " << p2.total << std::endl;

        cube.dump_nc();
        {
          std::ofstream out("nc.dot");
          cube.print_dot(out);
        }
        return false;
      }
    }
  }
  std::cerr << "could not find counterexample" << std::endl;
  return true;
}

#endif
