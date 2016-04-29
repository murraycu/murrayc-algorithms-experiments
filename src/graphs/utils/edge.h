#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EDGE
#define MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EDGE

#include <limits>

class Edge
{
public:
  using type_num = unsigned long;

  //Can be negative.
  using type_length = long;

  constexpr static type_length LENGTH_INFINITY = std::numeric_limits<type_length>::max();

  Edge()
  : destination_vertex_(0),
    length_(0)
  {}

  Edge(type_num destination_vertex, type_length length)
  : destination_vertex_(destination_vertex),
    length_(length)
  {}

  Edge(const Edge& src) = default;
  Edge& operator=(const Edge& src) = default;

  Edge(Edge&& src) = default;
  Edge& operator=(Edge&& src) = default;

  type_num destination_vertex_;
  type_length length_;
};

#endif // MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EDGE
