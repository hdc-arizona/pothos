#include <cstdlib>
#include <algorithm>
#include <utility>
#include <iostream>
#include <iterator>
#include <fstream>
#include <algorithm>

#include "nanocube.h"
#include "naivecube.h" // for test
#include "utils.h"
#include "tests.h"

namespace nc2 {

bool test_nanocube_1()
{
  int n_dims = 4;
  int dim_widths = 4;
  int n_points = 5;
  int n_tests = 1000;
  
  std::vector<size_t> widths;
  for (int i = 0; i < n_dims; ++i) {
    widths.push_back(dim_widths);
  }

  for (size_t k = 0; k < n_tests; ++k) {
    std::vector<std::pair<std::vector<size_t>, int> > values;
    for (size_t i = 0; i < n_points; ++i) {
      std::vector<size_t> p;
      for (size_t j = 0; j < n_dims; ++j)
        p.push_back(rand() % 8);
      values.push_back(std::make_pair(p, 1));
    }
    NanoCube<int>  nanocube (widths, values);
    NaiveCube<int> naivecube(widths, values);
    if (!test_randomized_query_equivalence(nanocube, naivecube, 1000)) {
      std::cerr << "points:" << std::endl;
      for (int i=0; i<values.size(); ++i) {
        std::cerr << stream_vector(values[i].first) << std::endl;
      }
      {
        std::ofstream of("nc.dot");
        nanocube.print_dot(of);
      }
      return false;
    }
  }
  return true;
}

bool test_naivecube_and_nanocube_equivalence_4()
{
  std::vector<size_t> widths { 3, 3, 3 };

  std::vector<std::pair<std::vector<size_t>, int> > values {
    std::make_pair(std::vector<size_t>({7, 1, 7}), 1),
    std::make_pair(std::vector<size_t>({6, 2, 3}), 1),
    std::make_pair(std::vector<size_t>({0, 0, 7}), 1),
    std::make_pair(std::vector<size_t>({5, 2, 6}), 1),
    std::make_pair(std::vector<size_t>({4, 1, 0}), 1),
    std::make_pair(std::vector<size_t>({4, 0, 1}), 1),
    std::make_pair(std::vector<size_t>({3, 0, 3}), 1),
    std::make_pair(std::vector<size_t>({4, 3, 1}), 1),
    std::make_pair(std::vector<size_t>({0, 2, 6}), 1),
    std::make_pair(std::vector<size_t>({1, 7, 6}), 1)
  };

  NanoCube<int>nanocube (widths, values);
  // {
  //   std::ofstream of("nc.dot");
  //   nanocube.print_dot(of);
  // }
  NaiveCube<int> naivecube(widths, values);
  return test_query_equivalence(nanocube, naivecube,
                                { std::make_pair(2, 7),
                                  std::make_pair(0, 5),
                                  std::make_pair(0, 6) });
  
}


bool test_naivecube_and_nanocube_equivalence_3()
{
  std::vector<size_t> widths { 4, 4 };

  std::vector<std::pair<std::vector<size_t>, int> > values {
    std::make_pair(std::vector<size_t>({10, 1}), 1),
    std::make_pair(std::vector<size_t>({0, 5}), 1),
    std::make_pair(std::vector<size_t>({2, 0}), 1),
    std::make_pair(std::vector<size_t>({10, 1}), 1),
    std::make_pair(std::vector<size_t>({13, 11}), 1),
    std::make_pair(std::vector<size_t>({5, 14}), 1),
    std::make_pair(std::vector<size_t>({9, 3}), 1),
    std::make_pair(std::vector<size_t>({4, 9}), 1),
    std::make_pair(std::vector<size_t>({7, 13}), 1),
    std::make_pair(std::vector<size_t>({12, 9}), 1)
  };

  NanoCube<int>  nanocube (widths, values);
  {
    std::ofstream of("nc.dot");
    nanocube.print_dot(of);
  }
  NaiveCube<int> naivecube(widths, values);
  return test_query_equivalence(nanocube, naivecube,
                                { std::make_pair(5, 13),
                                  std::make_pair(0, 12) });
  
}

bool test_naivecube_and_nanocube_equivalence()
{
  std::vector<size_t> widths { 4, 4 };
  for (size_t i=0; i<10000; ++i) {
    NaiveCube<int> naivecube(widths);
    NanoCube<int> nanocube(widths);
    std::vector<size_t> points;

    for (int j=0; j<10; ++j) {
      size_t v1 = rand() % 16, v2 = rand() % 16;
      points.push_back(v1);
      points.push_back(v2);
      std::vector<size_t> address { v1, v2 };
      naivecube.insert(address, 1);
      nanocube.insert(address, 1);
    }

    for (int j=0; j<10; ++j) {
      size_t v1 = rand() % 16, v2 = rand() % 16;
      size_t v3 = rand() % 16, v4 = rand() % 16;

      std::vector<std::pair<size_t, size_t> > range {
        std::make_pair(std::min(v1, v2), std::max(v1, v2)),
        std::make_pair(std::min(v3, v4), std::max(v3, v4)),
      };

      CombineSummaryPolicy<int> p1, p2;

      naivecube.range_query(p1, range);
      nanocube.range_query(p2, range);

      if (p1.total != p2.total) {
        std::cerr << "Failure! On dataset:" << std::endl;
        copy(points.begin(), points.end(),
             std::ostream_iterator<size_t>(std::cerr, " "));
        std::cerr << std::endl;
        std::cerr << "with query (("
                  << range[0].first << "," << range[0].second << "), ("
                  << range[1].first << "," << range[1].second << "))"
                  << std::endl;
        std::cerr << "data structures disagree: naivecube " << p1.total << " vs nanocube " << p2.total << std::endl;

        {
          std::ofstream out("nc.dot");
          nanocube.print_dot(out);
        }
        return false;
      }
    }
  }
  std::cerr << "could not find counterexample" << std::endl;
  return true;
}

bool test_naivecube_and_nanocube_equivalence_1()
{
  std::vector<size_t> widths { 4 };
  NaiveCube<int> naivecube(widths);
  NanoCube<int> nanocube(widths);
  std::vector<size_t> addresses { 7, 1, 9, 10, 2, 8, 8, 14, 3, 13 };
  for (auto &e: addresses) {
    naivecube.insert({e}, 1);
    nanocube.insert({e}, 1); 
  }

  std::vector<std::pair<size_t, size_t> > range { std::make_pair(5, 8) };

  CombineSummaryPolicy<int> p1, p2;
  naivecube.range_query(p1, range);
  nanocube.range_query(p2, range);

  if (p1.total != p2.total) {
    std::cerr << "Failure! On dataset:" << std::endl;
    copy(addresses.begin(), addresses.end(),
         std::ostream_iterator<size_t>(std::cerr, " "));
    std::cerr << std::endl;
    std::cerr << "with query (" << range[0].first << "," << range[0].second << ")" << std::endl;
    std::cerr << "data structures disagree: naivecube " << p1.total << " vs nanocube " << p2.total << std::endl;
    std::cerr << std::endl;
    nanocube.dump_nc();
    return false;
  }
    {
      std::ofstream out("nc.dot");
      nanocube.print_dot(out);
    }
  return true;
}

bool test_naivecube_and_nanocube_equivalence_2()
{
  std::vector<size_t> widths { 4, 4 };
  NaiveCube<int> naivecube(widths);
  NanoCube<int> nanocube(widths);
  std::vector<size_t> points;
  std::vector<std::vector<size_t> > addresses {
    {7, 1}, {9, 10}, {2, 8}, {8, 14}, {3, 13}, {8, 5}, {12, 2}, {3, 7}, {7, 1}, {8, 4}
  };
  for (auto &e: addresses) {
    points.push_back(e[0]);
    points.push_back(e[1]);
    naivecube.insert(e, 1);
    nanocube.insert(e, 1); 
  }

  // nanocube.dump_nc();
  // {
  //   std::ofstream out("nc.dot");
  //   nanocube.print_dot(out);
  // }

  std::vector<std::pair<size_t, size_t> > range {
    std::make_pair(1, 15),
    std::make_pair(5, 13)
        };

  CombineSummaryPolicy<int> p1, p2;
  naivecube.range_query(p1, range);
  nanocube.range_query(p2, range);

  if (p1.total != p2.total) {
    std::cerr << "Failure! On dataset:" << std::endl;
    copy(points.begin(), points.end(),
         std::ostream_iterator<size_t>(std::cerr, " "));
    std::cerr << std::endl;
    std::cerr << "with query (("
              << range[0].first << "," << range[0].second << "), ("
              << range[1].first << "," << range[1].second << "))"
              << std::endl;
    std::cerr << "data structures disagree: naivecube " << p1.total << " vs nanocube " << p2.total << std::endl;

    return false;
  }
  return true;
}

std::ostream &operator<<(std::ostream &os, const RefTrackedNCDimNode &n)
{
  os << n.l_ << ":" << n.r_ << ":" << n.n_;
  return os;
}

};
