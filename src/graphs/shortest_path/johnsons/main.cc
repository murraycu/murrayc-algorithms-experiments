#include "johnsons.h"
#include "utils/example_graphs.h"
#include <iostream>
#include <cstdlib>
#include <cassert>

int main()
{
  //TODO: This implementation seems to be incorrect.
  //It gets different values than the floyd warshall version.
  bool has_negative_cycles = false;
  const auto shortest_path_length =
    johnsons_all_pairs_shortest_path(EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES,
      has_negative_cycles);
  if(has_negative_cycles)
  {
    std::cout << "Negative cycle found." << std::endl;
  }
  else
  {
    std::cout << "Shortest path length: " << shortest_path_length << std::endl;
  }

  assert(shortest_path_length == -10003);

  return EXIT_SUCCESS;
}


