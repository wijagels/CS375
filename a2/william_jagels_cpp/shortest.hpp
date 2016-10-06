// Copyright William Jagels 2016
#pragma once
#include <cmath>
#include <string>
#include <tuple>
#include <vector>

void run_file(std::string, std::string);
namespace CS375 {
long dcount = 0;
typedef std::tuple<int, int, int> point;
typedef std::tuple<int, int, double> result_t;

double dist(const point&, const point&);
void print_cost(void);
result_t bf_shortest(const std::vector<point>&, int, int);
result_t dc_shortest(const std::vector<point>&);
result_t conqueror(const std::vector<point>&, int, int, int);
}
