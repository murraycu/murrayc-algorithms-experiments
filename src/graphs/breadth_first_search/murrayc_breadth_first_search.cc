#include "breadth_first_search.h"
#include "utils/example_graphs.h"
#include <iostream>
#include <cstdlib>
#include <cassert>

static
void test_small_dest(type_num source_vertex_num, type_num dest_vertex_num, const std::vector<SourceAndEdge>& expected_path)
{
  const auto shortest_path = bfs_compute_path(EXAMPLE_GRAPH_SMALL, source_vertex_num, dest_vertex_num);

  /*
  std::cout << "path from " << source_vertex_num << " to " << dest_vertex_num << ": ";
  for (auto i : shortest_path) {
    std::cout << i << ", ";
  }
  std::cout << std::endl;
  */

  assert(shortest_path == expected_path);
}

int main()
{
  test_small_dest(0, 0, {});
  test_small_dest(0, 1, {SourceAndEdge(0, 0)}); //0 to 1, using the first edge in 0.
  test_small_dest(1, 3, {SourceAndEdge(1, 1)}); //1 to 3, using the second edge in 1.
  test_small_dest(2, 3, {SourceAndEdge(2, 0)}); //2 to 3, using the first edge in 1.
  test_small_dest(0, 3, {
    SourceAndEdge(0, 0), //0 to 1, using the first edge in 0.
    SourceAndEdge(1, 1) //1 to 3, using the second edge in 1.
  });

  return EXIT_SUCCESS;
}


