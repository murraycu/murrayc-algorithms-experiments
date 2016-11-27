#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_BELLMAN_FORD
#define MURRAYC_ALGORITHMS_EXPERIMENTS_BELLMAN_FORD

#include "utils/shortest_path.h"
#include "utils/vertex.h"
#include <iostream>
#include <limits>
#include <vector>

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;

using type_num = Edge::type_num;
using type_length = Edge::type_length;

using type_shortest_paths = std::vector<type_length>;

using type_map_predecessors = std::vector<type_num>;

/**
 * Calculate the shortest path from @a s to @a v, using at most @i hops (edges).
 * @result true if the shortest path was changed.
 */
static bool
bellman_ford_update_adjacent_vertex(type_shortest_paths& shortest_paths,
  type_num w, const Edge& edge, type_length& shortest_path_so_far,
  type_map_predecessors& predecessors) {
  const auto v = edge.destination_vertex_;

  // Check the bounds:
  // Otherwise a bad edge in the input can cause an out-of-bounds access:
  if (v >= shortest_paths.size()) {
    std::cerr << "bellman_ford_update_adjacent_vertex(): v is too large: " << v
              << std::endl;
    return false;
  }

  // std::cout << "bellman_ford_update_adjacent_vertex(): i=" << i << ", s=" <<
  // s << ", w=" << w << ", v=" << v << std::endl;

  const auto case1 = shortest_paths[v];
  // std::cout << "  case1: existing: shortest_paths[i=" << i-1 << "][" << v <<
  // "]:" << case1 << std::endl;

  auto case2 = Edge::LENGTH_INFINITY; // min( w_to_v ) for all w.
  const auto direct_edge_length = edge.length_;
  const auto previous_s_w = shortest_paths[w];
  auto s_to_w_to_v = Edge::LENGTH_INFINITY;
  if (direct_edge_length != Edge::LENGTH_INFINITY &&
      previous_s_w != Edge::LENGTH_INFINITY) { // Avoid overflow
    // std::cout << "direct_edge_length: " << direct_edge_length << std::endl;
    // std::cout << "shortest_paths[w]: " <<
    // shortest_paths[w] << std::endl;
    s_to_w_to_v = previous_s_w + direct_edge_length;
    if (s_to_w_to_v < case2) {
      case2 = s_to_w_to_v;
    }
  }

  const auto best = std::min(case1, case2);

  // Make sure that it's less than any cost calculated for the same vertex
  // during this same iteration (i), for another other edge that goes to this
  // same vertex.
  bool changed = false;
  auto result = Edge::LENGTH_INFINITY;
  const auto existing = shortest_paths[v];
  if (existing < best) {
    result = existing;
  } else {
    result = best;

    // Also update the predecessor, if we've found a new best way to get to this
    // vertex:
    if (case2 < case1) {
      predecessors[v] = w;
    }

    changed = true;
  }

  // std::cout << "  result: " << result << std::endl;

  // Cache it:
  // std::cout << "  storing: shortest_paths[i=" << i << "][" << v << "]:" <<
  // result << std::endl;
  shortest_paths[v] = result;

  if (result < shortest_path_so_far) {
    shortest_path_so_far = result;
  }

  // std::cout << "bellman_ford_update_adjacent_vertex: i=" << i << ", s=" << s
  // << ", v=" << v << ", shortest_path_so_far: " << shortest_path_so_far <<
  // std::endl;

  return changed;
}

/**
 * Calculate the shortest path from @a s to @a v, using at most @i hops (edges).
 * @result true if the shortest path was changed.
 */
static bool
bellman_ford_update_for_vertex(const type_vec_nodes& vertices,
  type_shortest_paths& shortest_paths, type_num v,
  type_length& shortest_path_so_far, type_map_predecessors& predecessors) {
  bool changed = false;
  for (const auto& edge : vertices[v].edges_) {
    // bellman_ford_update_adjacent_vertex will use the existing shortest_paths
    // and write new values in shortest_paths.
    const auto this_changed = bellman_ford_update_adjacent_vertex(
      shortest_paths, v, edge, shortest_path_so_far, predecessors);
    if (this_changed) {
      changed = true;
    }
  }

  return changed;
}

/**
 * @result true if at least one shortest path was changed.
 */
static bool
bellman_ford_single_iteration(const type_vec_nodes& vertices,
  type_shortest_paths& shortest_paths, type_length& shortest_path_so_far,
  type_map_predecessors& predecessors) {
  bool changed = false;
  const auto vertices_count = vertices.size();
  for (type_num v = 0; v < vertices_count; ++v) {
    // Relax the shortest path to this vertex:
    const auto this_changed = bellman_ford_update_for_vertex(
      vertices, shortest_paths, v, shortest_path_so_far, predecessors);
    if (this_changed) {
      changed = true;
    }
  }

  return changed;
}

/** Get all the shortests paths from s to all other paths.
 * This will be more efficient if the caller firsts removes excess parallel
 * edges,
 * leaving only the lowest-cost edge between each vertex.
 */
std::vector<ShortestPath>
bellman_ford_single_source_shortest_paths(
  const type_vec_nodes& vertices, type_num s, bool& has_negative_cycles) {
  // Initialize output variable:
  has_negative_cycles = false;

  // n:
  const auto vertices_count = vertices.size();

  type_map_predecessors map_path_predecessor(vertices_count);
  map_path_predecessor[s] = 0; // Invalid

  // Initialize shortest paths:
  // - 0 to get to the source from the source.
  // - Infinity to get to any other vertex in 0 edges.
  type_shortest_paths shortest_paths(vertices_count, Edge::LENGTH_INFINITY);
  shortest_paths[s] = 0;

  auto shortest_path_so_far = Edge::LENGTH_INFINITY;

  // i is the number of steps for which we are calculating the shortest path.
  // We need n-1 iterations.
  for (type_num i = 0; i < (vertices_count - 1); ++i) {
    // std::cout << "i=" << i << std::endl;
    const auto changed = bellman_ford_single_iteration(
      vertices, shortest_paths, shortest_path_so_far, map_path_predecessor);

    // If the shortest paths so far have not changed, then they won't change
    // next time, so we can finish early:
    if (!changed) {
      // std::cout << "Finishing early at i=" << i <<
      //  ", count=" << vertices_count << std::endl;
      break;
    }
  }

  const auto shortest_path = shortest_path_so_far;

  // Check for a negative cycle:
  // Try one more calculation, this time with i=n.
  // If we get a shorter path then we must have taken a negative cycle:
  bellman_ford_single_iteration(
    vertices, shortest_paths, shortest_path_so_far, map_path_predecessor);

  if (shortest_path_so_far < shortest_path) {
    has_negative_cycles = true;
  }

  std::vector<ShortestPath> result;
  type_num end_vertex = 0;
  for (const auto& length_and_predecessor : shortest_paths) {
    const auto length = length_and_predecessor;

    // Examine the chain of predecessors to get the full path:
    std::string path;
    auto predecessor = end_vertex;
    while (predecessor != s) {
      path = std::to_string(predecessor) + ", " + path;
      predecessor = map_path_predecessor[predecessor];
    }

    path = std::to_string(s) + ", " + path;

    result.emplace_back(length, path);

    ++end_vertex;
  }
  return result;
}

#endif // MURRAYC_ALGORITHMS_EXPERIMENTS_BELLMAN_FORD
