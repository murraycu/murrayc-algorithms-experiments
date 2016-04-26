#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_VERTEX
#define MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_VERTEX

#include "utils/edge.h"
#include <vector>

class Vertex
{
public:
  Vertex()
  {}

  explicit Vertex(std::initializer_list<Edge>&& edges)
  : edges_(std::move(edges))
  {}

  Vertex(const Vertex& src) = default;
  Vertex& operator=(const Vertex& src) = default;

  Vertex(Vertex&& src) = default;
  Vertex& operator=(Vertex&& src) = default;

  // A list of other vertices that the vertex has edges leading to.
  std::vector<Edge> edges_;
};

#endif // MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_VERTEX
