#include "utils/example_graphs.h"
#include "bellman_ford.h"
#include <iostream>
#include <cstdlib>

int main()
{
  const auto start_vertex = 0;
  const auto dest_vertices = {0, 1, 2, 3};

  bool has_negative_cycles = false;
  const auto shortest_paths =
    bellman_ford_single_source_shortest_paths(EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES,
      start_vertex, has_negative_cycles);

  for(auto dest_vertex_num : dest_vertices)
  {
    const auto shortest_path = shortest_paths[dest_vertex_num];
    std::cout << "shortest path from " << start_vertex << " to " << dest_vertex_num
      << ": " << shortest_path.length_
      << ", path: " << shortest_path.path_ << std::endl;
  }

  return EXIT_SUCCESS;
}


