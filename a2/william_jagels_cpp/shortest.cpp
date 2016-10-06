// Copyright William Jagels 2016
#include "shortest.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include "./debug.h"

void test_1(void);
void test_2(void);

int main(int argc, char* argv[]) {
  // CS375::dcount = 0;
  // test_1();
  // CS375::dcount = 0;
  // test_2();
  if (argc != 3) {
    std::cerr << "Invalid command line arguments" << std::endl;
    return 1;
  }
  run_file(std::string(argv[1]), std::string(argv[2]));
  return 0;
}

void run_file(std::string in, std::string out) {
  std::ifstream in_f(in);
  if (!in_f.is_open()) {
    loge("Fatal error, file %s unopenable.", in.c_str());
    exit(1);
  }
  std::vector<CS375::point> pts;
  std::string line;
  while (std::getline(in_f, line)) {
    std::stringstream ss{line};
    std::string x_s, y_s, z_s;
    getline(ss, x_s, ' ');
    getline(ss, y_s, ' ');
    int x = atoi(x_s.c_str());
    int y = atoi(y_s.c_str());
    int z = 0;
    if (getline(ss, z_s, ' ')) {
      z = atoi(z_s.c_str());
    }
    pts.push_back(std::make_tuple(x, y, z));
  }
#ifndef NBRUTE_SHORTEST
  std::cout
      << "Starting brute force (compile with -DNBRUTE_SHORTEST to disable)"
      << std::endl;
  CS375::bf_shortest(pts, 0, pts.size());
  CS375::print_cost();
#endif
  std::cout << "Running D&C algo" << std::endl;
  CS375::result_t res = CS375::dc_shortest(pts);
  CS375::print_cost();
  std::ofstream output(out);
  output << std::get<0>(res) << " " << std::get<1>(res) << " "
         << std::get<2>(res) << std::endl;
}

/*
void test_1(void) {
  std::vector<CS375::point> pts;
  int n = 10000;
  for (int i = 0; i < n; i++) {
    pts.push_back(std::make_tuple(n - i, n - i, 0));
  }
  CS375::result_t r = CS375::dc_shortest(pts);
  std::cout << CS375::dcount << std::endl;
  std::cout << std::get<0>(r) << "," << std::get<1>(r) << "," << std::get<2>(r)
            << std::endl;
  CS375::dcount = 0;
  CS375::result_t bf = CS375::bf_shortest(pts, 0, pts.size());
  std::cout << CS375::dcount << std::endl;
  assert(std::get<2>(r) == std::get<2>(bf));
}

void test_2(void) {
  std::vector<CS375::point> pts;
  int n = 10000;
  for (int i = 0; i < n; i++) {
    pts.push_back(std::make_tuple(i * i, i * i, i));
  }
  CS375::result_t r = CS375::dc_shortest(pts);
  std::cout << CS375::dcount << std::endl;
  std::cout << std::get<0>(r) << "," << std::get<1>(r) << "," << std::get<2>(r)
            << std::endl;
  CS375::dcount = 0;
  CS375::result_t bf = CS375::bf_shortest(pts, 0, pts.size());
  std::cout << CS375::dcount << std::endl;
  assert(std::get<2>(r) == std::get<2>(bf));
}
*/

namespace CS375 {
double dist(const point& a, const point& b) {
  ++dcount;
  double i = std::get<0>(a) - std::get<0>(b);
  double j = std::get<1>(a) - std::get<1>(b);
  double k = std::get<2>(a) - std::get<2>(b);
  return std::sqrt(i * i + j * j + k * k);
}

void print_cost(void) {
  std::cout << "Distance operations: " << dcount << std::endl;
  dcount = 0;
}

result_t bf_shortest(const std::vector<point>& pts, int lower, int upper) {
  assert(upper - lower > 1);
  result_t min = std::make_tuple(0, 1, dist(pts.at(0), pts.at(1)));
  for (int i = lower; i < upper; i++) {
    for (int j = i + 1; j < upper; j++) {
      double r = dist(pts.at(i), pts.at(j));
      if (r < std::get<2>(min)) {
        min = std::make_tuple(i, j, r);
      }
    }
  }
  return min;
}

/*
 * Book keeping function, gets the ball rolling
 */
result_t dc_shortest(const std::vector<point>& pts) {
  std::vector<std::pair<size_t, point> > smap;
  // Create a mapping of sorted index to unsorted
  for (size_t i = 0; i < pts.size(); i++) {
    smap.push_back(std::make_pair(i, pts.at(i)));
    // logd("%zu, %d", i, std::get<0>(pts.at(i)));
  }
  // Sort according to the first dimension, O(n lg n)
  std::sort(smap.begin(), smap.end(), [](const std::pair<size_t, point>& a,
                                         const std::pair<size_t, point>& b) {
    return std::get<0>(a.second) < std::get<0>(b.second);
  });
  std::vector<point> sorted_points;
  for (auto it : smap) {
    sorted_points.push_back(it.second);
    // logd("%zu", it.first);
  }
  // Conquer the list, O(n lg n)
  result_t answer = conqueror(sorted_points, 0, sorted_points.size(), 0);
  // Transform the indexes back
  std::get<0>(answer) = smap.at(std::get<0>(answer)).first;
  std::get<1>(answer) = smap.at(std::get<1>(answer)).first;
  return answer;
}

result_t conqueror(const std::vector<point>& pts, int lower, int upper, int d) {
  logd("%d, %d, %d", d, lower, upper);
  if (upper - lower <= 2) {
    if (upper - 1 == lower) lower--;
    return std::make_tuple(lower, upper - 1,
                           dist(pts.at(lower), pts.at(upper - 1)));
  }
  int midpoint = (upper + lower) / 2;
  logd("Mp: %d", midpoint);
  result_t left = conqueror(pts, lower, midpoint, d + 1);
  result_t right = conqueror(pts, midpoint, upper, d + 1);
  // Determine minimum distance
  double min = std::min(std::get<2>(left), std::get<2>(right));
  logd("Min: %f {%f, %f}", min, std::get<2>(left), std::get<2>(right));
  if (min > std::numeric_limits<double>::min()) {
    int up = midpoint;
    int low = midpoint;
    while (std::get<0>(pts.at(up)) < std::get<0>(pts.at(midpoint)) + min &&
           up < upper - 1)
      up++;
    while (std::get<0>(pts.at(low)) > std::get<0>(pts.at(midpoint)) + min &&
           low > lower + 1)
      low--;
    if (up - low > 1) {
      result_t middle = bf_shortest(pts, low, up);
      logd("%d, %d, %d, %f", d, std::get<0>(middle), std::get<1>(middle),
           std::get<2>(middle));
      logd("%d, %d, %d, %f", d, std::get<0>(left), std::get<1>(left),
           std::get<2>(left));
      logd("%d, %d, %d, %f", d, std::get<0>(right), std::get<1>(right),
           std::get<2>(right));
      return std::min({left, right, middle},
                      [](const result_t& a, const result_t& b) {
                        return std::get<2>(a) < std::get<2>(b);
                      });
    }
  }

  logd("%d, %d, %d, %f", d, std::get<0>(left), std::get<1>(left),
       std::get<2>(left));
  logd("%d, %d, %d, %f", d, std::get<0>(right), std::get<1>(right),
       std::get<2>(right));
  return std::min({left, right},
                  [](const result_t& a, const result_t& b) {
                    return std::get<2>(a) < std::get<2>(b);
                  });
}
}  // namespace CS375
