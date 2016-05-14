#include "breadth_first_search.h"
#include "utils/example_graphs.h"
#include <iostream>
#include <cstdlib>
#include <cassert>

static
void test_small_dest(type_num source_vertex_num, type_num dest_vertex_num, const std::vector<type_num>& expected_path)
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
  test_small_dest(0, 0, {0});
  test_small_dest(0, 1, {0, 1});
  test_small_dest(1, 3, {1, 3});
  test_small_dest(2, 3, {2, 3});
  test_small_dest(0, 3, {0, 1, 3});

  return EXIT_SUCCESS;
}


