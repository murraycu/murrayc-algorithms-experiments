#include "utils/vertex.h"
#include "utils/example_graphs.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <fstream>

static std::string
build_dot_file(const type_vec_nodes& vertices) {
  std::string result = "digraph example {\n";

  const auto n = vertices.size();
  for (std::size_t i = 0; i < n; ++i) {
    const auto& v = vertices[i];
    for (const auto e : v.edges_) {
      result += "  " + std::to_string(i) + " -> " + std::to_string(e.destination_vertex_)
        + "[label=" + std::to_string(e.length_) + "]\n";
    }
  }

  result += "}";

  return result;
}

/**
 * These files can then be converted to pictures of graphs, like so:
 * $ dot -Tpdf example_graph_small.dot -o test.pdf
 */
static void
output_dot_file(const type_vec_nodes& vertices, const std::string& filename) {
  const auto str = build_dot_file(vertices);
  std::ofstream o(filename);
  o << str;
}

int
main() {
  output_dot_file(EXAMPLE_GRAPH_SMALL, "example_graph_small.dot");
  output_dot_file(EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES, "example_graph_small_with_negative_edges.dot");
  output_dot_file(EXAMPLE_GRAPH_LARGER_WITH_NEGATIVE_EDGES, "example_graph_larger_with_negative_edges.dot");

  const std::vector<Vertex> g1 = {
    Vertex({Edge(1, 1), Edge(2, 1)}),
    Vertex({Edge(2, 1)}),
    Vertex()};
  output_dot_file(g1, "testg1.dot");

  const std::vector<Vertex> g2 = {
    Vertex({Edge(1, 1), Edge(2, 1)}),
    Vertex(),
    Vertex({Edge(1, 1)})};
  output_dot_file(g2, "testg2.dot");

  return EXIT_SUCCESS;
}
