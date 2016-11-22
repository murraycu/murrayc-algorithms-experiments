#include "kruskals.h"
#include "utils/example_graphs.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <numeric>

static void
test_mst(const std::vector<Vertex>& graph, type_length expected_cost) {
  const auto msts = compute_mst_cost(graph);
  if (msts.size() == 1) {
    std::cout << "There is a minimum spanning tree:" << std::endl;
  } else {
    std::cout << "There is not a single minimum spanning tree:" << std::endl;
  }

  /*
    for (const auto& mst : msts) {
      type_length cost = 0;
      for (const auto& edge : mst) {
        std::cout << "edge: " << edge.source_vertex_ << " to " <<
    edge.destination_vertex_
          << ", cost=" << edge.length_ << std::endl;
        cost += edge.length_;
      }

      std::cout << "  MST cost: " << cost << std::endl;
    }
  */

  assert(msts.size() == 1);
  const auto& mst = msts[0];
  const auto cost = std::accumulate(mst.begin(), mst.end(), 0,
    [](auto sum, const auto& edge) { return sum + edge.length_; });
  std::cout << "MST cost: " << cost << std::endl;
  assert(cost == expected_cost);
}

int
main() {
  test_mst(EXAMPLE_GRAPH_SMALL, 6);
  test_mst(EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES, -10013);
  test_mst(EXAMPLE_GRAPH_LARGER_WITH_NEGATIVE_EDGES,
    7); // TODO: Not the same as with prims.

  return EXIT_SUCCESS;
}
