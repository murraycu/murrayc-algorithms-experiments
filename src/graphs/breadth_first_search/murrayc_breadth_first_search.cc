#include "breadth_first_search.h"
#include "utils/example_graphs.h"
#include <iostream>
#include <cstdlib>
#include <cassert>

static
void test_small_dest(type_num source_vertex_num, type_num dest_vertex_num, const std::vector<type_num>& expected_path)
{
  std::vector<type_num> path_vertices;

  type_vec_path path;
  if (bfs_compute_path(EXAMPLE_GRAPH_SMALL, source_vertex_num, dest_vertex_num,
    path)) {
    path_vertices = get_vertices_for_path(source_vertex_num, path, EXAMPLE_GRAPH_SMALL);
  }

  /*
  std::cout << "path from " << source_vertex_num << " to " << dest_vertex_num << ": ";
  for (auto i : path_vertices) {
    std::cout << i << ", ";
  }
  std::cout << std::endl;
  */

  assert(path_vertices == expected_path);
}

int main()
{
/*
  test_small_dest(0, 0, {});
  test_small_dest(0, 1, {SourceAndEdge(0, 0)}); //0 to 1, using the first edge in 0.
  test_small_dest(1, 3, {SourceAndEdge(1, 1)}); //1 to 3, using the second edge in 1.
  test_small_dest(2, 3, {SourceAndEdge(2, 0)}); //2 to 3, using the first edge in 1.
  test_small_dest(0, 3, {
    SourceAndEdge(0, 0), //0 to 1, using the first edge in 0.
    SourceAndEdge(1, 1) //1 to 3, using the second edge in 1.
  });
*/

  test_small_dest(0, 0, {0});
  test_small_dest(0, 1, {0, 1});
  test_small_dest(1, 3, {1, 3});
  test_small_dest(2, 3, {2, 3});
  test_small_dest(0, 3, {0, 1, 3});

  return EXIT_SUCCESS;
}


