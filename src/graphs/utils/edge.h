#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EDGE
#define MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EDGE

#include <limits>

class Edge {
public:
  using type_num = unsigned long;

  // Can be negative.
  using type_length = long;

  // TODO: Use a constexpr here instead of a static variable.
  // But, with g++ 6.2, the linker complains that it is undefined
  // when using -O0 to disable optimization.
  static const type_length LENGTH_INFINITY;

  Edge() : destination_vertex_(0), length_(0), reverse_edge_in_dest_(0) {}

  Edge(type_num destination_vertex, type_length length)
  : destination_vertex_(destination_vertex),
    length_(length),
    reverse_edge_in_dest_(0) {}

  Edge(const Edge& src) = default;
  Edge&
  operator=(const Edge& src) = default;

  Edge(Edge&& src) = default;
  Edge&
  operator=(Edge&& src) = default;

  type_num destination_vertex_;
  type_length length_; // Or cost, capacity, etc.

  // TODO: This is only used for max-flow algorithms.
  type_num reverse_edge_in_dest_;
};

const Edge::type_length Edge::LENGTH_INFINITY = std::numeric_limits<type_length>::max();

#endif // MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EDGE
