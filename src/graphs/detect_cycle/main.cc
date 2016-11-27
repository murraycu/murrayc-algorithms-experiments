#include "detect_cycle.h"
#include "utils/vertex.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

static void
test_recursive_without_cycle() {
  assert(!detect_cycle_recursive(EXAMPLE_GRAPH_SMALL));
  assert(!detect_cycle_recursive(EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES));

  // 1 vertex that links to 2 vertices, one of which links to the other.
  const std::vector<Vertex> g1 = {
    Vertex({Edge(1, 1), Edge(2, 1)}),
    Vertex({Edge(2, 1)}),
    Vertex()};
  assert(!detect_cycle_recursive(g1));

  // 1 vertex that links to 2 vertices, one of which links to the other.
  // (A mirror of the g1).
  const std::vector<Vertex> g2 = {
    Vertex({Edge(1, 1), Edge(2, 1)}),
    Vertex(),
    Vertex({Edge(1, 1)})};
  assert(!detect_cycle_recursive(g2));
}

static void
test_recursive_with_cycle() {
  assert(detect_cycle_recursive(EXAMPLE_GRAPH_LARGER_WITH_NEGATIVE_EDGES));
}

static void
test_recursive_from_source_with_cycle() {
  assert(detect_cycle_recursive(EXAMPLE_GRAPH_LARGER_WITH_NEGATIVE_EDGES, 0));
}

static void
test_iterative_without_cycle() {
  assert(!detect_cycle_iterative(EXAMPLE_GRAPH_SMALL));
  assert(!detect_cycle_iterative(EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES));

  // 1 vertex that links to 2 vertices, one of which links to the other.
  const std::vector<Vertex> g1 = {
    Vertex({Edge(1, 1), Edge(2, 1)}),
    Vertex({Edge(2, 1)}),
    Vertex()};
  assert(!detect_cycle_iterative(g1));

  // 1 vertex that links to 2 vertices, one of which links to the other.
  // (A mirror of the g1).
  const std::vector<Vertex> g2 = {
    Vertex({Edge(1, 1), Edge(2, 1)}),
    Vertex(),
    Vertex({Edge(1, 1)})};
  assert(!detect_cycle_iterative(g2));
}

static void
test_iterative_with_cycle() {
  assert(detect_cycle_iterative(EXAMPLE_GRAPH_LARGER_WITH_NEGATIVE_EDGES));
}

static void
test_iterative_from_source_with_cycle() {
  assert(detect_cycle_iterative(EXAMPLE_GRAPH_LARGER_WITH_NEGATIVE_EDGES, 0));
}

int
main() {
  test_recursive_without_cycle();
  test_recursive_with_cycle();
  test_recursive_from_source_with_cycle();

  test_iterative_without_cycle();
  test_iterative_with_cycle();
  test_iterative_from_source_with_cycle();

  return EXIT_SUCCESS;
}
