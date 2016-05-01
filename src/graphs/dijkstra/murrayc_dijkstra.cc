#include "dijkstra.h"
#include "utils/example_graphs.h"
#include <iostream>
#include <cstdlib>
#include <cassert>

static
void test_small_dest(type_num dest_vertex_num, Edge::type_length expected_shortest_path)
{
  const auto shortest_path = dijkstra_compute_shortest_path(EXAMPLE_GRAPH_SMALL, 0, dest_vertex_num);
  assert(shortest_path.length_ == expected_shortest_path);
}

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

  test_small_dest(0, 0);
  test_small_dest(1, 3);
  test_small_dest(2, 3);
  test_small_dest(3, 5);

  return EXIT_SUCCESS;
}


