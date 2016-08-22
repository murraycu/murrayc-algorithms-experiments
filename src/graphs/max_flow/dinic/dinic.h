#include "utils/example_graphs.h"
#include "shortest_path/breadth_first_search/breadth_first_search.h"
#include <stack>
#include <queue>
#include <iostream>

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;

using type_num = Edge::type_num;

using type_depths = std::vector<type_num>;

static
type_depths
bfs_calculate_levels(type_num start_vertex, const type_vec_nodes& vertices)
{
  const auto vertices_size = vertices.size();
  type_depths result(vertices_size);

  std::queue<type_num> queue;
  queue.emplace(start_vertex);
  std::vector<bool> discovered(vertices_size);
  discovered[start_vertex] = true;

  result[start_vertex] = 0;

  while(!queue.empty()) {
    const auto i = queue.front();
    queue.pop();
    const auto& vertex = vertices[i];
    const auto depth = result[i];

    const auto& edges = vertex.edges_;
    const auto edges_count = edges.size();
    for (type_num e = 0; e < edges_count; ++e) {
      const auto& edge = edges[e];

      //Ignore zero-length edges.
      if (edge.length_ == 0) {
        continue;
      }

      const auto edge_dest = edge.destination_vertex_;
      if (discovered[edge_dest])
        continue;

      discovered[edge_dest] = true;
      queue.emplace(edge_dest);

      result[edge_dest] = depth + 1;
    }
  }

  return result;
}


static
type_vec_nodes
make_residual_graph(const type_vec_nodes& vertices)
{
  type_vec_nodes result = vertices;

  const auto vertices_count = result.size();
  for (type_num i = 0; i < vertices_count; ++i) {
    auto& vertex = result[i];

    const auto edges_count = vertex.edges_.size();
    for (type_num e = 0; e < edges_count; ++e) {
      const auto& edge = vertex.edges_[e];

      const auto dest_vertex_num = edge.destination_vertex_;
      auto& dest_vertex = result[dest_vertex_num];
      auto& edges = dest_vertex.edges_;

      //Add reverse edge:
      edges.emplace_back(i, 0);
      auto& reverse = edges.back();

      //Tell each edge about its reverse edge:
      reverse.reverse_edge_in_dest_ = e;

      auto& source_edge = vertex.edges_[e];
      source_edge.reverse_edge_in_dest_ = edges.size() - 1;
    }
  }

  return result;
}

static
Edge& get_reverse_edge(const Edge& edge, type_vec_nodes& vertices)
{
  auto& dest = vertices[edge.destination_vertex_];
  return dest.edges_[edge.reverse_edge_in_dest_];
}


bool
dfs_find_path(const type_vec_nodes& vertices,
  const type_depths& levels, type_num max_level,
  type_num start_vertex, type_num dest_vertex, type_vec_path& path)
{
  type_vec_path result;

  const auto vertices_size = vertices.size();

  if (start_vertex >= vertices_size) {
    std::cerr << "start vertex not found in vertices: " << start_vertex << std::endl;
    path = result;
    return false;
  }

  if (dest_vertex >= vertices_size) {
    std::cerr << "end vertex not found in vertices: " << dest_vertex << std::endl;
    path = result;
    return false;
  }

  std::vector<bool> discovered(vertices_size);
  std::unordered_map<type_num, SourceAndEdge> predecessors;

  std::stack<type_num> stack;
  stack.emplace(start_vertex);
  discovered[start_vertex] = true;

  bool found = false;

  while(!stack.empty()) {
    const auto i = stack.top();
    stack.pop();

    //Only DFS down to a certain level:
    if (levels[i] > max_level) {
      continue;
    }

    const auto& vertex = vertices[i];

    if (i == dest_vertex) {
      found = true;
      break;
    }

    const auto& edges = vertex.edges_;
    const auto edges_count = edges.size();
    for (type_num e = 0; e < edges_count; ++e) {
      const auto& edge = edges[e];

      //Ignore zero-length edges.
      if (edge.length_ == 0) {
        continue;
      }

      const auto edge_dest = edge.destination_vertex_;
      if (discovered[edge_dest])
        continue;

      discovered[edge_dest] = true;
      stack.emplace(edge_dest);

      //std::cout << "predecessor of " << edge_dest << " is " << i << std::endl;
      predecessors[edge_dest] = SourceAndEdge(i, e);
    }
  }

  if (found)
    result = get_path_from_predecessors(start_vertex, dest_vertex, predecessors);

  path = result;
  return found;
}

/** Find and augment the blocking flows for the @a k depth level of the
 * residual graph, returning the amount of flow added.
 */
bool
dfs_find_blocking_flow(type_vec_nodes& residual_graph,
  const type_depths& levels, type_num max_level,
  type_num start_vertex, type_num dest_vertex, Edge::type_length& flow)
{
  flow = 0;

  bool at_least_one_path_found = false;
  type_vec_path path;
  while(dfs_find_path(residual_graph, levels, max_level,
    start_vertex, dest_vertex, path)) {
    at_least_one_path_found = true;

    //const auto path_vertices = get_vertices_for_path(start_vertex, path, residual_graph);
    //for (const auto& v : path_vertices) {
    //  std::cout << v << ", ";
    //}
    //std::cout << std::endl;

    //Find the bottleneck in this path
    //(The edge with the smallest remaining capacity.)
    const auto iter = std::min_element(path.begin(), path.end(),
      [&residual_graph] (const auto& a, const auto& b) {
        const auto& a_vertex = residual_graph[a.source_];
        const auto& a_edge = a_vertex.edges_[a.edge_];

        const auto& b_vertex = residual_graph[b.source_];
        const auto& b_edge = b_vertex.edges_[b.edge_];

        return a_edge.length_ < b_edge.length_;
      });
    const auto& min_vertex = residual_graph[iter->source_];
    const auto& min_edge = min_vertex.edges_[iter->edge_];
    const auto c = min_edge.length_;
    //std::cout << "c: " << c << std::endl;

    //Augment the path:
    for (auto& source_and_edge : path) {
      auto& source = residual_graph[source_and_edge.source_];
      auto& edge = source.edges_[source_and_edge.edge_];

      //Reduce the edge's capacity:
      //std::cout << "before:"
      //  << ", capacity=" << edge.length_
      //  << ", residual=" << edge.residual_ << std::endl;
      edge.length_ -= c;
      //std::cout << "after: edge.length_=" << edge.length_ << std::endl;

      //Increase the reverse edge's capacity, to allow an undo:
      auto& reverse_edge = get_reverse_edge(edge, residual_graph);
      reverse_edge.length_ += c;
    }

    flow += c;
  }

  return at_least_one_path_found;
}


/** This is actually the Edmonds-Karp algorithm,
 * because it uses BFS to find the path in each iteration, giving us the path
 * with the least number of hops in each iteration.
 * Using DFS would still be Ford-Fulkerson, but not Edmunds-Karp, and would
 * be less efficient.
 */
static
Edge::type_length
dinic_max_flow(const type_vec_nodes& vertices, type_num source_vertex_num, type_num sink_vertex_num)
{
  Edge::type_length result = 0;

  auto residual_graph = make_residual_graph(vertices);
  const auto levels = bfs_calculate_levels(source_vertex_num, residual_graph);
  //for (type_num i = 0; i < levels.size(); ++i) {
  //  std::cout << "vertex " << i << ": level " << levels[i] << std::endl;
  //}

  //Find the blocking paths for each level:
  const auto max_iter = std::max_element(levels.begin(), levels.end());
  const auto max_level = *max_iter;
  for (type_num k = 1; k <= max_level; ++k) {
    type_vec_path path;
    Edge::type_length flow = 0;
    while(dfs_find_blocking_flow(residual_graph, levels, k,
      source_vertex_num, sink_vertex_num,
      flow)) {
      result += flow;
    }
  }

  return result;
}
