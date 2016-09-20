#include "push_relabel.h"
#include "utils/example_graphs.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

static void
test_small(type_num source_vertex_num, type_num sink_vertex_num,
  Edge::type_length expected_max_flow) {
  const auto max_flow = push_relabel_max_flow(
    EXAMPLE_GRAPH_SMALL_FOR_FLOW, source_vertex_num, sink_vertex_num);

  std::cout << "max flow from " << source_vertex_num << " to "
            << sink_vertex_num << ": " << max_flow << std::endl;
  assert(max_flow == expected_max_flow);
}

int
main() {
  // TODO: Test a graph that actually needs us to use the residual (reverse)
  // edges to undo.
  test_small(0, 3, 5);
  test_small(0, 2, 5);
  test_small(2, 3, 3);

  return EXIT_SUCCESS;
}
