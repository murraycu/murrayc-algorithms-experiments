#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_DIJKSTRA
#define MURRAYC_ALGORITHMS_EXPERIMENTS_DIJKSTRA

#include "utils/shortest_path.h"
#include "utils/vertex.h"
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;

using type_num = Edge::type_num;
using type_length = Edge::type_length;

/**
 * The element for our heap.
 */
class VertexAndLength {
public:
  VertexAndLength() : vertex_(0), total_length_(0), predecessor_(0) {}

  VertexAndLength(
    type_num vertex, type_length total_length, type_num predecessor)
  : vertex_(vertex), total_length_(total_length), predecessor_(predecessor) {}

  type_num vertex_;
  type_length total_length_;
  type_num predecessor_;
};

static_assert(std::is_copy_assignable<VertexAndLength>::value,
  "VertexAndLength should be copy assignable.");
static_assert(std::is_copy_constructible<VertexAndLength>::value,
  "VertexAndLength should be copy constructible.");
static_assert(std::is_move_assignable<VertexAndLength>::value,
  "VertexAndLength should be move assignable.");
static_assert(std::is_move_constructible<VertexAndLength>::value,
  "VertexAndLength should be move constructible.");

static std::string
get_path_from_predecessors(type_num start_vertex, type_num end_vertex,
  const std::unordered_map<type_num, type_num>& map_path_predecessor) {
  std::string path;

  type_num predecessor = end_vertex;
  while (predecessor != start_vertex) {
    path = std::to_string(predecessor) + ", " + path;

    const auto iter = map_path_predecessor.find(predecessor);
    if (iter == map_path_predecessor.end()) {
      // This is normal if there is no path to this vertex.
      /*
      std::cerr << "get_path_from_predecessors(): predecessor not found for
      vertex:"
        << predecessor
        << ", start_vertex=" << start_vertex << ", end_vertex=" << end_vertex <<
      std::endl;
      */
      break;
    }

    if (predecessor == iter->second) {
      std::cerr << "get_path_from_predecessors(): avoiding infinite loop."
                << std::endl;
      break;
    }

    predecessor = iter->second;
  }

  path = std::to_string(start_vertex) + ", " + path;
  return path;
}

static std::vector<ShortestPath>
dijkstra_compute_shortest_paths(const type_vec_nodes& vertices,
  type_num start_vertex, type_num end_vertex, bool stop_at_end = false) {
  std::vector<ShortestPath> result;

  const auto vertices_size = vertices.size();

  if (start_vertex >= vertices_size) {
    std::cerr << "start vertex not found in vertices: " << start_vertex
              << std::endl;
    return result;
  }

  if (end_vertex >= vertices_size) {
    std::cerr << "end vertex not found in vertices: " << end_vertex
              << std::endl;
    return result;
  }

  // std::cout << " dijkstra_compute_shortest_path(): start_vertex: " <<
  // start_vertex << ", end_vertex: " << end_vertex << std::endl;
  // TODO: A simple vector would probably be more efficient,
  // as long as we need to explore almost all the vertices.
  std::unordered_map<type_num, type_num> map_path_predecessor;
  map_path_predecessor[start_vertex] = 0; // Invalid

  // TODO: A simple vector would probably be more efficient,
  // as long as we need to explore almost all the vertices.
  std::unordered_set<type_num> explored;

  const auto comparator = [](
    const auto& a, const auto& b) { return a.total_length_ > b.total_length_; };
  std::priority_queue<VertexAndLength, std::vector<VertexAndLength>,
    decltype(comparator)>
    heap(comparator);
  heap.emplace(start_vertex, 0, 0);

  // Optionally, store the shortest paths for all destination vertices:
  std::unordered_map<type_num, VertexAndLength> total_lengths;

  while (!heap.empty()) {
    // Get the vertex which has an edge leading to it from the explored set,
    // of minimum length:
    const auto best = heap.top();
    heap.pop();
    const auto best_vertex = best.vertex_;

    if (explored.count(best_vertex) > 0) {
      // std::cout << "    best_vertex is in explored." << std::endl;
      // This must be an invalid entry in the heap,
      // which we left to check later, instead of removing.
      // Ignore it.
      continue;
    }

    map_path_predecessor[best_vertex] = best.predecessor_;

    // Optionally, remember the shortest path to all destination vertices:
    if (!stop_at_end) {
      total_lengths.emplace(best_vertex, best);
    }

    // Optionally, stop if we've found the destination vertex.
    // No other path can be shorter,
    // because every part of the path must be the shortest way to get to that
    // part.
    // Or, optionally, continue to get the shortest paths to all vertices.
    if (stop_at_end && best_vertex == end_vertex) {
      const auto path = get_path_from_predecessors(
        start_vertex, end_vertex, map_path_predecessor);
      result.emplace_back(best.total_length_, path);
      return result;
    }

    // Move it to explored:
    explored.emplace(best_vertex);

    const auto& vertex = vertices[best_vertex];
    for (const auto& edge : vertex.edges_) {
      const auto edge_destination_vertex = edge.destination_vertex_;

      // Ignore edges leading into the already-explored set:
      if (explored.count(edge_destination_vertex) > 0)
        continue;

      const auto total_length = best.total_length_ + edge.length_;
      heap.emplace(
        edge_destination_vertex, total_length, best_vertex /* predecessor */);
    }
  }

  result.reserve(vertices_size);
  for (type_num i = 0; i < vertices_size; ++i) {
    const auto& dest_shortest = total_lengths[i];
    const auto path =
      get_path_from_predecessors(start_vertex, i, map_path_predecessor);
    result.emplace_back(dest_shortest.total_length_, path);
  }

  return result;
}

std::vector<ShortestPath>
dijkstra_compute_shortest_paths(
  const type_vec_nodes& vertices, type_num start_vertex) {
  return dijkstra_compute_shortest_paths(vertices, start_vertex,
    0 /* not used, when passing false */, false /* stop at end_vertex */);
}

ShortestPath
dijkstra_compute_shortest_path(
  const type_vec_nodes& vertices, type_num start_vertex, type_num end_vertex) {
  const auto shortest_paths = dijkstra_compute_shortest_paths(
    vertices, start_vertex, end_vertex, true /* stop at end_vertex */);
  if (shortest_paths.empty()) {
    std::cerr << "dijkstra_compute_shortest_path(): "
                 "dijkstra_compute_shortest_path() failed."
              << std::endl;
    return ShortestPath(0, "");
  }

  return shortest_paths[0];
}

#endif // MURRAYC_ALGORITHMS_EXPERIMENTS_DIJKSTRA
