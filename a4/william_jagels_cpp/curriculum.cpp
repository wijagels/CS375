#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <utility>
#include <vector>

#include "WeightedDirectedGraph.hpp"

struct Node {
  Node() : Node{-1} {}
  Node(int data) : data{data}, outgoing{}, incoming{} {}
  int data;
  std::vector<Node*> outgoing;
  std::vector<Node*> incoming;
};

void connect(Node* a, Node* b) {
  if (b->data == 0) return;
  if (b->data == -1) return;
  a->outgoing.push_back(b);
  b->incoming.push_back(a);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Invalid number of arguments" << std::endl;
    return 1;
  }
  std::ifstream input{argv[1]};
  if (!input.is_open()) {
    std::cerr << "Failed to open file" << std::endl;
    return 1;
  }
  std::array<Node, 16> nodes;
  for (size_t i = 0; i < nodes.size(); i++) {
    nodes[i] = Node{(int)i};
  }
  std::string line;
  while (getline(input, line)) {
    std::stringstream ss{line};
    int from, to;
    ss >> from;
    ss >> to;
    connect(&nodes[from], &nodes[to]);
  }
  std::array<bool, 16> visited{{false}};
  visited[0] = true;
}

int bfs(std::array<bool, 16>& visited, Node* cur) {
  int level = 0;
  if (visited[cur->data]) return 0;
  for (auto n : cur->incoming) {
    if (!visited[n->data]) {
      return 0;
    }
  }
  std::queue<Node*> q;
  visited[cur->data] = true;
  for (auto n : cur->outgoing) {
    q.push(n);
  }
  while (!q.empty()) {
    auto v = q.front();
    q.pop();
    visited[v->data] = true;
    for (auto el : v->incoming) {
      if (!visited[el->data]) {
        visited[v->data] = false;
      }
    }
    if (visited[v->data]) continue;
  }
  return level;
}
