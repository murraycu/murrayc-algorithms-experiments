#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EDGE
#define MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_EDGE

class Edge
{
public:
  using type_num = unsigned long;
  using type_length = unsigned long;

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
