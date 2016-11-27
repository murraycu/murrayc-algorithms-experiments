#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_BELLMAN_FORD_WITH_QUEUE
#define MURRAYC_ALGORITHMS_EXPERIMENTS_BELLMAN_FORD_WITH_QUEUE

#include "detect_cycle/detect_cycle.h"
#include "utils/shortest_path.h"
#include "utils/vertex.h"
#include <iostream>
#include <limits>
#include <queue>
#include <stack>
#include <vector>

using type_num = Edge::type_num;
using type_length = Edge::type_length;

using type_shortest_paths = std::vector<type_length>;

using type_map_predecessors = std::vector<type_num>;
constexpr type_num INVALID_PREDECESSOR = std::numeric_limits<type_num>::max();

/**
 * DFS to discover any negative cycle so far
 */
static bool
check_has_negative_cycle(const type_vec_nodes& vertices, type_num s,
  const type_map_predecessors& predecessors) {

  // Build a graph that is just the shortest path tree so far:
  // Well, it should be a tree, but it could have a cycle,
  // which would be negative, which we will then find.
  const auto n = vertices.size();
  type_vec_nodes g(n);
  for (std::size_t i = 0; i < n; ++i) {
    const auto pred = predecessors[i];
    if (pred != INVALID_PREDECESSOR) {
      g[pred].edges_.emplace_back(i, 1); // The weight doesn't matter here.
    }
  }

  // DFS on the tree to find a cycle.
  return detect_cycle(g, s);
}

/**
 * Calculate the shortest path from @a s to @a v, using at most @i hops (edges).
 */
static void
bellman_ford_update_adjacent_vertex(std::queue<type_num>& q,
  std::vector<bool>& on_q, type_shortest_paths& shortest_paths, type_num w,
  const Edge& edge, type_map_predecessors& predecessors) {
  const auto v = edge.destination_vertex_;

  // Check the bounds:
  // Otherwise a bad edge in the input can cause an out-of-bounds access:
  if (v >= shortest_paths.size()) {
    std::cerr << "bellman_ford_update_adjacent_vertex(): v is too large: " << v
              << std::endl;
    return;
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

    if (!on_q[v]) {
      q.emplace(v);
      on_q[v] = true;
    }
  }

  // std::cout << "  result: " << result << std::endl;

  // Cache it:
  // std::cout << "  storing: shortest_paths[i=" << i << "][" << v << "]:" <<
  // result << std::endl;
  shortest_paths[v] = result;

  // std::cout << "bellman_ford_update_adjacent_vertex: i=" << i << ", s=" << s
  // << ", v=" << v << ", shortest_path_so_far: " << shortest_path_so_far <<
  // std::endl;
}

/**
 * "Relax" the path to v.
 */
static void
bellman_ford_update_for_vertex(std::queue<type_num>& q, std::vector<bool>& on_q,
  const type_vec_nodes& vertices, type_shortest_paths& shortest_paths,
  type_num v, type_map_predecessors& predecessors) {
  for (const auto& edge : vertices[v].edges_) {
    // bellman_ford_update_adjacent_vertex will use the existing shortest_paths
    // and write new values in shortest_paths.
    bellman_ford_update_adjacent_vertex(
      q, on_q, shortest_paths, v, edge, predecessors);
  }
}

/** Get all the shortests paths from s to all other paths.
 * This will be more efficient if the caller firsts removes excess parallel
 * edges,
 * leaving only the lowest-cost edge between each vertex.
 *
 * This version, using a queue, is slightly more efficient than the regulat
 * version, because, in each (implicit) iteration, it only checks vertices
 * whose paths changed (were relaxed) last time.
 * However, I wonder if cache considerations (data locality) makes the standard
 * algorithm perform better in practice.
 * Performance can be improved by only checking for a cycle every now and then,
 * for instance after every n pops from the stack.
 *
 * This is based on Sedgewick/Wayne's implementation in
 * Algorithms (4th edition), page 674.
 * That book doesn't say so, but this might be the implementation described
 * here:
 * http://wcipeg.com/wiki/Shortest_Path_Faster_Algorithm
 * which says that this is inaccurate, at the time of writing:
 * https://en.wikipedia.org/wiki/Shortest_Path_Faster_Algorithm
 * However, I think I've also read somewhere that this was Bellman's original
 * algorithm.
 */
std::vector<ShortestPath>
bellman_ford_single_source_shortest_paths_with_queue(
  const type_vec_nodes& vertices, type_num s, bool& has_negative_cycles) {
  // Initialize output variable:
  has_negative_cycles = false;

  // n:
  const auto vertices_count = vertices.size();

  type_map_predecessors map_path_predecessor(
    vertices_count, INVALID_PREDECESSOR);

  // Initialize shortest paths:
  // - 0 to get to the source from the source.
  // - Infinity to get to any other vertex in 0 edges.
  type_shortest_paths shortest_paths(vertices_count, Edge::LENGTH_INFINITY);
  shortest_paths[s] = 0;

  // A queue of vertices,
  // and whether the vertex is on the queue:
  std::vector<bool> on_q(vertices_count);
  std::queue<type_num> q;
  q.emplace(s);

  while (!q.empty()) {
    // std::cout << "i=" << i << std::endl;
    const auto v = q.front();
    q.pop();

    // Relax the shortest path to this vertex:
    bellman_ford_update_for_vertex(
      q, on_q, vertices, shortest_paths, v, map_path_predecessor);
    if (check_has_negative_cycle(vertices, s, map_path_predecessor)) {
      has_negative_cycles = true;
      return {};
    }
  }

  // Check for a negative cycle:
  // Try one more calculation, this time with i=n.
  // If we get a shorter path then we must have taken a negative cycle:
  // TODO: bellman_ford_single_iteration(q, on_q, vertices, shortest_paths,
  //  map_path_predecessor);

  // if (shortest_path_so_far < shortest_path) {
  //  has_negative_cycles = true;
  //}

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

#endif // MURRAYC_ALGORITHMS_EXPERIMENTS_BELLMAN_FORD_WITH_QUEUE
