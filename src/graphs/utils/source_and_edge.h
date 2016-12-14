#ifndef ford_MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_SOURCE_AND_EDGE
#define ford_MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_SOURCE_AND_EDGE

#include "utils/edge.h"
#include "utils/vertex.h"

class SourceAndEdge {
public:
  using type_num = Edge::type_num;

  SourceAndEdge() : source_(0), edge_(0) {}

  SourceAndEdge(type_num source, type_num edge)
  : source_(source), edge_(edge) {}

  bool
  operator==(const SourceAndEdge& src) const {
    return source_ == src.source_ && edge_ == src.edge_;
  }

  type_num source_;
  type_num edge_;
};

static_assert(std::is_copy_assignable<SourceAndEdge>::value,
  "SourceAndEdge should be copy assignable.");
static_assert(std::is_copy_constructible<SourceAndEdge>::value,
  "SourceAndEdge should be copy constructible.");
static_assert(std::is_move_assignable<SourceAndEdge>::value,
  "SourceAndEdge should be move assignable.");
static_assert(std::is_move_constructible<SourceAndEdge>::value,
  "SourceAndEdge should be move constructible.");

using type_vec_path = std::vector<SourceAndEdge>;

/**
 * Get just the vertices for the path,
 * including the start and destination vertices.
 * If @a path is empty, this is interpreted as a path that begins and ends at
 * the @a start_vertex.
 *
 * @path The path, in terms of edges.
 */
std::vector<Edge::type_num>
get_vertices_for_path(Edge::type_num start_vertex, const type_vec_path& path,
  const type_vec_nodes& vertices) {
  std::vector<Edge::type_num> result;
  result.reserve(path.size() + 1);

  if (path.empty()) {
    result.emplace_back(start_vertex);
  }

  for (const auto& source_and_edge : path) {
    if (result.empty()) {
      result.emplace_back(source_and_edge.source_);
    }

    const auto& vertex = vertices[source_and_edge.source_];
    const auto& edge = vertex.edges_[source_and_edge.edge_];
    const auto dest_vertex_num = edge.destination_vertex_;
    result.emplace_back(dest_vertex_num);
  }

  return result;
}

#endif // ford_MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_SOURCE_AND_EDGE
