#include "bellman_ford.h"
#include "bellman_ford_with_queue.h"
#include "utils/example_graphs.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

static void
test_bellman_ford() {
  const auto start_vertex = 0;
  const auto dest_vertices = {0, 1, 2, 3};

  bool has_negative_cycles = false;
  const auto shortest_paths = bellman_ford_single_source_shortest_paths(
    EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES, start_vertex, has_negative_cycles);

  for (auto dest_vertex_num : dest_vertices) {
    const auto& shortest_path = shortest_paths[dest_vertex_num];
    std::cout << "shortest path from " << start_vertex << " to "
              << dest_vertex_num << ": " << shortest_path.length_
              << ", path: " << shortest_path.path_ << std::endl;
  }

  assert(!has_negative_cycles);
  assert(!shortest_paths.empty());

  assert(shortest_paths[0].length_ == 0);
  assert(shortest_paths[1].length_ == -5);
  assert(shortest_paths[2].length_ == -4);
  assert(shortest_paths[3].length_ == -3);
}

static void
test_bellman_ford_with_queue() {
  const auto start_vertex = 0;
  const auto dest_vertices = {0, 1, 2, 3};

  bool has_negative_cycles = false;
  const auto shortest_paths =
    bellman_ford_single_source_shortest_paths_with_queue(
      EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES, start_vertex,
      has_negative_cycles);

  for (auto dest_vertex_num : dest_vertices) {
    const auto& shortest_path = shortest_paths[dest_vertex_num];
    std::cout << "shortest path from " << start_vertex << " to "
              << dest_vertex_num << ": " << shortest_path.length_
              << ", path: " << shortest_path.path_ << std::endl;
  }

  assert(!has_negative_cycles);
  assert(!shortest_paths.empty());

  assert(shortest_paths[0].length_ == 0);
  assert(shortest_paths[1].length_ == -5);
  assert(shortest_paths[2].length_ == -4);
  assert(shortest_paths[3].length_ == -3);
}

int
main() {
  test_bellman_ford();
  test_bellman_ford_with_queue();

  return EXIT_SUCCESS;
}
