#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EXAMPLE_GRAPHS
#define MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EXAMPLE_GRAPHS

#include "utils/edge.h"
#include "utils/vertex.h"

// 0-indexed vertices:
const std::vector<Vertex> EXAMPLE_GRAPH_SMALL = {
  Vertex({Edge(1, 3), Edge(2, 3)}), Vertex({Edge(2, 1), Edge(3, 2)}),
  Vertex({Edge(3, 50)}), Vertex()};

// 0-indexed vertices:
const std::vector<Vertex> EXAMPLE_GRAPH_SMALL_FOR_FLOW = {
  Vertex({Edge(1, 3), Edge(2, 2)}), Vertex({Edge(2, 5), Edge(3, 2)}),
  Vertex({Edge(3, 3)}), Vertex()};

const std::vector<Vertex> EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES = {
  Vertex({Edge(5, -10), Edge(1, -5)}), Vertex({Edge(2, 1)}),
  Vertex({Edge(3, 1)}), Vertex({Edge(4, -10000)}), Vertex(), Vertex()};

const std::vector<Vertex> EXAMPLE_GRAPH_LARGER_WITH_NEGATIVE_EDGES = {
  Vertex({Edge(9, -4), Edge(9, 3), Edge(6, 6), Edge(7, 17)}),
  Vertex({Edge(5, 16), Edge(8, 17), Edge(5, 2), Edge(0, -2), Edge(7, 15)}),
  Vertex({Edge(4, 23), Edge(1, 18)}),
  Vertex({Edge(1, 11), Edge(9, 7), Edge(5, 4), Edge(5, 21), Edge(1, 13)}),
  Vertex({Edge(7, 3), Edge(4, 11), Edge(3, 1), Edge(9, 8), Edge(9, 2)}),
  Vertex({Edge(2, 21), Edge(0, 18), Edge(3, -2), Edge(7, 15)}),
  Vertex({Edge(2, 3), Edge(9, 22), Edge(1, -1)}),
  Vertex({Edge(5, 17), Edge(4, 24)}),
  Vertex({Edge(2, 8), Edge(0, 18), Edge(2, 7)}),
  Vertex({Edge(8, 20), Edge(8, 17), Edge(8, 24), Edge(6, 24), Edge(3, 10),
    Edge(3, 13)})};

#endif // MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EXAMPLE_GRAPHS
