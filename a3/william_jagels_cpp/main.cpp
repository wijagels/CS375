// Copyright William Jagels 2016
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include "WeightedDirectedGraph.hpp"

void floyd(cs375::WeightedDirectedGraph<std::string>, std::string, std::string);

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Invalid number of arguments" << std::endl;
    return 1;
  }
  std::ifstream input(argv[1]);
  if (!input.is_open()) {
    std::cerr << "Failed to open file" << std::endl;
    return 1;
  }
  cs375::WeightedDirectedGraph<std::string> g{
      {"New York City", "Allentown", "Scranton", "Binghamton", "Rochester",
       "Buffalo", "Syracuse", "Albany", "Toronto", "Kingston", "Montreal"}};
  std::string line;
  while (getline(input, line)) {
    std::stringstream ss{line};
    std::string weight_s;
    std::string from;
    std::string to;
    getline(ss, weight_s, ';');
    getline(ss, from, ';');
    getline(ss, to, ';');
    int weight = atoi(weight_s.c_str());
    g.update_edge(from, to, weight);
    g.update_edge(to, from, weight);
  }
  floyd(g, "New York City", "Toronto");
}

void floyd(cs375::WeightedDirectedGraph<std::string> graph, std::string from,
           std::string to) {
  std::pair<bool, int> def{false, 0};
  std::vector<std::string> result;
  result.push_back(from);
  std::vector<std::vector<std::pair<bool, int> > >& D = graph.ad_matrix_;
  std::vector<std::vector<std::string> > P{graph.idx_, {graph.idx_, "NIL"}};
  for (auto i : graph) {
    for (auto j : graph) {
      if (i != j && graph.distance(i.first, j.first).first)
        P.at(i.second).at(j.second) = i.first;
    }
  }
  for (auto kr : graph) {
    for (auto ir : graph) {
      for (auto jr : graph) {
        int i = ir.second;
        int j = jr.second;
        auto dist = graph.distance(ir.first, kr.first);
        if (!dist.first) continue;
        auto dist_2 = graph.distance(kr.first, jr.first);
        if (!dist_2.first) continue;
        int dist_total = dist_2.second + dist.second;
        int min = D.at(i).at(j).second;
        if (dist_total < min || D.at(i).at(j).first == false) {
          min = dist_total;
          P[i][j] = P[kr.second][j];
        }
        D.at(i).at(j) = {true, min};
      }
    }
  }
  auto b = graph.index_of(from);
  auto e = graph.index_of(to);
  std::cout << from << std::endl;
  while (b != e) {
    std::cout << P.at(e).at(b) << std::endl;
    b = graph.index_of(P.at(e).at(b));
  }
}
