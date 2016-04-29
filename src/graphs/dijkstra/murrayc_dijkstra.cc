#include "dijkstra.h"
#include "utils/example_graphs.h"
#include <iostream>
#include <cstdlib>

int main()
{
  const auto start_vertex = 0;
  const auto dest_vertices = {0, 1, 2, 3};
  for(auto dest_vertex_num : dest_vertices)
  {
    const auto shortest_path = dijkstra_compute_shortest_path(EXAMPLE_GRAPH_SMALL, start_vertex, dest_vertex_num);
    std::cout << "shortest path from " << start_vertex << " to " << dest_vertex_num
      << ": " << shortest_path.length_
      << ", path: " << shortest_path.path_ << std::endl;
  }

  return EXIT_SUCCESS;
}


