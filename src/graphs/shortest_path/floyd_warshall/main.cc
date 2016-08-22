#include "utils/example_graphs.h"
#include "floyd_warshall.h"
#include <iostream>
#include <cstdlib>
#include <cassert>

static
void test_apsp(const std::vector<Vertex>& graph, type_length expected_shortest_path)
{
  bool has_negative_cycles = false;
  const auto shortest_path_length =
    floyd_warshall_calc_all_pairs_shortest_path(graph, has_negative_cycles);
  if(has_negative_cycles)
  {
    std::cout << "Negative cycle found." << std::endl;
  }
  else
  {
    std::cout << "Shortest path length: " << shortest_path_length << std::endl;
  }

  assert(shortest_path_length == expected_shortest_path);
}

int main()
{
  test_apsp(EXAMPLE_GRAPH_SMALL, 0);
  test_apsp(EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES, -10003);
  test_apsp(EXAMPLE_GRAPH_LARGER_WITH_NEGATIVE_EDGES, -4);

  return EXIT_SUCCESS;
}


