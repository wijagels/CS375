#pragma once
// Copyright William Jagels 2016
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cs375 {

/*
 * This class is a set of Vertices with edges connecting them, each of which has
 * a weight. The edges are stored as a pair<bool, int> inside an adjacency
 * matrix. This class is probably bad for graphs with low connectivity, as it
 * takes up O(|V|^2) space. Note that this class does not support erasing
 * vertices.
 */
template <typename Key_T>
class WeightedDirectedGraph {
 public:
  WeightedDirectedGraph() : idx_{0}, ad_matrix_{}, vertices_{} {}
  WeightedDirectedGraph(std::initializer_list<Key_T> verts)
      : WeightedDirectedGraph{} {
    for (auto v : verts) {
      add_vertex(v);
    }
  }

  /*
   * Adds a vertex and allocates another row and column in the adjacency matrix.
   * This function takes O(|V|) time to expand the matrix.
   */
  bool add_vertex(const Key_T &vertex) {
    auto r = vertices_.emplace(vertex, idx_);
    if (!r.second) {
      // Perform no action if the vertex exists
      return r.second;
    }
    ++idx_;
    // Grow the matrix to the right
    for (auto &row : ad_matrix_) {
      row.push_back({false, 0});
    }
    // Grow the matrix downwards
    ad_matrix_.emplace_back(idx_, std::pair<bool, int>{false, 0});
    return r.second;
  }

  /*
   * Updates the weight of the edge from->to. In order to remove parallel edges,
   * this function will set the weight to the minimum of the weight passed in
   * and the current weight. This function takes O(1) time.
   */
  void update_edge(const Key_T &from, const Key_T &to, int weight) {
    if (from == to) return;
    size_t f_idx = index_of(from);
    size_t t_idx = index_of(to);
    auto &ref = ad_matrix_.at(f_idx).at(t_idx);
    if (ref.first) {
      ref.second = std::min<size_t>(ref.second, weight);
    } else {
      ref.first = true;
      ref.second = weight;
    }
  }

  /*
   * Deletes the edge connecting from->to. Has no effect if from == to. Time
   * complexity is O(1)
   */
  void remove_edge(const Key_T &from, const Key_T &to) {
    if (from == to) return;
    size_t f_idx = index_of(from);
    size_t t_idx = index_of(to);
    auto ref = ad_matrix_.at(f_idx).at(t_idx);
    ref = {false, 0};
  }

  /*
   * Looks up the weight of the edge connecting from->to. Will return {true, 0}
   * if from and to are equal. Returns {false, 0} in the event that there is no
   * edge connecting from->to. Otherwise, if the edge exists it will return
   * {true, x} where x is the weight of the edge connecting from->to. This
   * operation takes O(1) time.
   */
  std::pair<bool, int> distance(const Key_T &from, const Key_T &to) const {
    if (from == to) {
      // Each vertex is connected to itself with weight zero
      return {true, 0};
    }
    size_t f_idx = index_of(from);
    size_t t_idx = index_of(to);
    return ad_matrix_.at(f_idx).at(t_idx);
  }

  typename std::unordered_map<Key_T, size_t>::iterator begin() {
    return vertices_.begin();
  }

  typename std::unordered_map<Key_T, size_t>::iterator end() {
    return vertices_.end();
  }

  typename std::unordered_map<Key_T, size_t>::const_iterator begin() const {
    return vertices_.begin();
  }

  typename std::unordered_map<Key_T, size_t>::const_iterator end() const {
    return vertices_.end();
  }

  /*
   * Looks up the location of a vertex in the adjacency matrix. Throws
   * std::out_of_range if the vertex does not exist.
   */
  size_t index_of(const Key_T &vertex) const {
    auto search = vertices_.find(vertex);
    if (search == vertices_.end())
      throw std::out_of_range("Vertex does not exist");
    return search->second;
  }

  size_t idx_;
  std::vector<std::vector<std::pair<bool, int>>> ad_matrix_;
  std::unordered_map<Key_T, size_t> vertices_;
};
}  // namespace cs375
