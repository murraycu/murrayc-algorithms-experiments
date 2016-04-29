#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EXAMPLE_GRAPHS
#define MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EXAMPLE_GRAPHS

#include "utils/edge.h"
#include "utils/vertex.h"

//0-indexed vertices:
const std::vector<Vertex> EXAMPLE_GRAPH_SMALL = {
  Vertex({Edge(1, 3), Edge(2, 3)}),
  Vertex({Edge(2, 1), Edge(3, 2)}),
  Vertex({Edge(3, 50)}),
  Vertex()
};

const std::vector<Vertex> EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES = {
  Vertex({Edge(5, -10), Edge(1, -5)}),
  Vertex({Edge(2, 1)}),
  Vertex({Edge(3, 1)}),
  Vertex({Edge(4, -10000)}),
  Vertex(),
  Vertex()
};

#endif // MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EXAMPLE_GRAPHS
