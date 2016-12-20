#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_VERTEX
#define MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_VERTEX

#include "utils/edge.h"
#include <vector>

class Vertex {
public:
  Vertex() {}

  explicit Vertex(std::initializer_list<Edge>&& edges)
  : edges_(std::move(edges)) {}

  // A list of other vertices that the vertex has edges leading to.
  std::vector<Edge> edges_;
};

static_assert(
  std::is_copy_assignable<Vertex>::value, "Vertex should be copy assignable.");
static_assert(std::is_copy_constructible<Vertex>::value,
  "Vertex should be copy constructible.");
static_assert(
  std::is_move_assignable<Vertex>::value, "Vertex should be move assignable.");
static_assert(std::is_move_constructible<Vertex>::value,
  "Vertex should be move constructible.");

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;

#endif // MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_VERTEX
