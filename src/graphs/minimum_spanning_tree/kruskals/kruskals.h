#include "union_find.h"
#include "utils/vertex.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;
using type_vec_edges = std::vector<Edge>;

using type_num = Edge::type_num;
using type_length = Edge::type_length;

class EdgeWithSource : public Edge {
public:
  EdgeWithSource(const Edge& edge, type_num source_vertex)
  : Edge(edge), source_vertex_(source_vertex) {}

  EdgeWithSource(
    type_num source_vertex, type_num destination_vertex, type_length length)
  : Edge(destination_vertex, length), source_vertex_(source_vertex) {}

  type_num source_vertex_;
};

static_assert(std::is_copy_assignable<EdgeWithSource>::value,
  "EdgeWithSource should be copy assignable.");
static_assert(std::is_copy_constructible<EdgeWithSource>::value,
  "EdgeWithSource should be copy constructible.");
static_assert(std::is_move_assignable<EdgeWithSource>::value,
  "EdgeWithSource should be move assignable.");
static_assert(std::is_move_constructible<EdgeWithSource>::value,
  "EdgeWithSource should be move constructible.");

using type_vec_edges_with_sources = std::vector<EdgeWithSource>;

using type_set_msts = std::vector<type_vec_edges_with_sources>;

/** Erase an item from a map, returning its value.
 */
template <typename T_Map>
typename T_Map::mapped_type
get_and_erase_from_map(T_Map& map, const typename T_Map::key_type& key) {
  auto iter = map.find(key);
  if (iter == map.end()) {
    using ValueType = typename T_Map::mapped_type;
    return ValueType();
  }

  const auto result = iter->second;
  map.erase(iter);
  return result;
}

/**
 * @param max_clusters Call this with 1 to get a single Minimum Spanning Tree.
 */
static type_set_msts
find_clusters(const type_vec_edges_with_sources& sorted_edges,
  type_num count_nodes, type_num max_clusters) {
  type_set_msts result; // Declare this here to try to get some RVO.

  // Map of roots to spanning tree costs:
  std::unordered_map<type_num, type_vec_edges_with_sources> map_msts;

  UnionFind<type_num> ds(count_nodes);
  type_num clusters_count = count_nodes;
  type_length min_spacing = std::numeric_limits<type_length>::max();

  for (const auto& edge : sorted_edges) {
    const auto& from = edge.source_vertex_;
    const auto& to = edge.destination_vertex_;
    // std::cout << "from=" << from << ", to=" << to << std::endl;

    const auto from_leader = ds.find_set(from);
    const auto to_leader = ds.find_set(to);

    /*
    if(from_leader == 0) {
      ds.make_set(from);
    }

    if(to_leader == 0) {
      ds.make_set(to);
    }
    */

    // If they were not already in a cluster,
    if (from_leader != to_leader) {
      // std::cout << "clusters_count=" << clusters_count << ", max_clusters="
      // << max_clusters << std::endl;
      if (clusters_count > max_clusters) {
        // std::cout << "  joining" << std::endl;

        // Get and remove existing cost for these clusters/trees:
        // TODO: This would be easier if our UnionFind data structure could
        // store associated data, like a map.
        auto existing_cost_from =
          get_and_erase_from_map(map_msts, ds.find_set(from));
        auto existing_cost_to =
          get_and_erase_from_map(map_msts, ds.find_set(to));

        ds.union_set(from, to);

        // Update the cost for this cluster/tree:
        // Concatenate the list of edges, and the new edge.
        std::copy(existing_cost_to.begin(), existing_cost_to.end(),
          std::back_inserter(existing_cost_from));
        existing_cost_from.emplace_back(edge);
        map_msts[ds.find_set(from)] = existing_cost_from;

        // There is now one less cluster:
        clusters_count--;
      } else {
        // This is a single-item cluster:
        // Add the vertex that is not in a cluster already:
        // This is not a real edge, but it's how we tell the caller
        // that there is only one node in the cluster.
        if (from == from_leader && !map_msts.count(from)) {
          map_msts[from] = {EdgeWithSource(from, from, 0)};
        }

        if (to == to_leader && !map_msts.count(to)) {
          map_msts[to] = {EdgeWithSource(to, to, 0)};
        }

        // std::cout << "  remembering max distance" << std::endl;
        // Remember the biggest spacing between remaining clusters:
        if (edge.length_ < min_spacing) {
          min_spacing = edge.length_;
        }
      }
    } else {
      // These edges are already joined.
      // std::cout << "  already joined:" << std::endl;
    }
  }

  // std::cout << "min_spacing: " << min_spacing << std::endl;

  // Return only the costs of the clusters/trees,
  // not including the UnionFind's roots:
  std::transform(map_msts.begin(), map_msts.end(),
    std::inserter(result, result.end()),
    [](const auto& a) { return a.second; });
  return result;
}

static type_set_msts
compute_mst_cost(const type_vec_nodes& vertices) {
  type_vec_edges_with_sources edges;
  const auto vertices_count = vertices.size();
  for (type_num v = 0; v < vertices_count; ++v) {
    const auto& vertex_edges = vertices[v].edges_;

    std::transform(vertex_edges.begin(), vertex_edges.end(),
      std::back_inserter(edges),
      [v](const auto& edge) { return EdgeWithSource(edge, v); });
  }

  // Sort the edges by their distance (ascending):
  std::sort(edges.begin(), edges.end(),
    [](const auto& a, const auto& b) -> bool { return a.length_ < b.length_; });

  return find_clusters(edges, vertices.size(), 1);
}
