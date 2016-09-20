#include "prims.h"
#include "utils/example_graphs.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

int
main() {
  auto cost = compute_mst_cost(EXAMPLE_GRAPH_SMALL);
  std::cout << "MST cost: " << cost << std::endl;
  assert(cost == 6);

  cost = compute_mst_cost(EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES);
  std::cout << "MST cost: " << cost << std::endl;
  assert(cost == -10013);

  cost = compute_mst_cost(EXAMPLE_GRAPH_LARGER_WITH_NEGATIVE_EDGES);
  std::cout << "MST cost: " << cost << std::endl;
  assert(cost == 59);

  return EXIT_SUCCESS;
}
