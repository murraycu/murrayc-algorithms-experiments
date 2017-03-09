#include "utils/vertex.h"
#include <iostream>
#include <limits>
#include <queue>
#include <unordered_set>

using type_num = Edge::type_num;

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;
using type_set_nodes = std::unordered_set<type_num>;

using type_length = Edge::type_length;

const auto comparator = [](
  const auto& a, const auto& b) { return a.length_ > b.length_; };
using type_pq =
  std::priority_queue<Edge, std::vector<Edge>, decltype(comparator)>;

static void
add_edges_to_pq(type_pq& pq_edges, const type_vec_nodes& nodes,
  const type_set_nodes& mst_nodes, type_num node_num) {
  const auto& node = nodes[node_num];

  for (const auto& edge : node.edges_) {
    // Ignore edges that don't lead out of the current tree:
    if (mst_nodes.count(edge.destination_vertex_)) {
      continue;
    }

    pq_edges.emplace(edge);
  }
}

static type_length
compute_mst_cost(const type_vec_nodes& nodes) {
  // We track the nodes in the MST just to know when an edge's destination is
  // out of the tree.
  type_set_nodes mst_nodes;

  // We track the edges in the MST so we can sum their lengths,
  // though we could do this using a simple numeric sum along the way.
  std::vector<Edge> mst_edges;

  // We use a priority queue to always find the lowest-cost edge out of the MST.
  type_pq pq_edges(comparator);

  const auto start_node_num = 0;
  // std::cout << "start_node: " << start_node_num << std::endl;
  mst_nodes.emplace(start_node_num);
  add_edges_to_pq(pq_edges, nodes, mst_nodes, start_node_num);

  while (!pq_edges.empty()) {
    // Find the outgoing edge from the current tree's nodes with the smallest
    // cost:
    auto edge = pq_edges.top();
    pq_edges.pop();

    const auto node_num = edge.destination_vertex_;

    // Ignore edges that don't lead out.
    // We need to do this because we add shorter edges without removing (or
    // decreasing) old ones.
    // A Fiboncci heap would let us decrease-key in amortized O(1) time instead.
    if (mst_nodes.count(node_num)) {
      continue;
    }

    mst_nodes.emplace(node_num);
    mst_edges.emplace_back(edge);

    add_edges_to_pq(pq_edges, nodes, mst_nodes, node_num);
  }

  // Calculate the complete cost of the edges:
  type_length cost = 0;
  for (const auto& edge : mst_edges) {
    cost += edge.length_;
  }

  return cost;
}
