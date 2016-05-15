#include "utils/example_graphs.h"
#include "breadth_first_search/breadth_first_search.h"
#include <iostream>
#include <cstdlib>
#include <cassert>

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;

using type_num = Edge::type_num;

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

static
Edge::type_length
ford_fulkerson_max_flow(const type_vec_nodes& vertices, type_num source_vertex_num, type_num sink_vertex_num)
{
  Edge::type_length result = 0;

  //TODO: Separate map of edges to their flows?
  auto residual_graph = make_residual_graph(vertices);

  type_vec_path path;
  while(bfs_compute_path(residual_graph, source_vertex_num, sink_vertex_num,
    path)) {
    
    //std::cout << "path found: ";
    //const auto path_vertices = get_vertices_for_path(source_vertex_num, path, residual_graph);
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
    
    result += c;
  }

  return result;
}

static
void test_small(type_num source_vertex_num, type_num sink_vertex_num, Edge::type_length expected_max_flow)
{
  const auto max_flow = ford_fulkerson_max_flow(EXAMPLE_GRAPH_SMALL_FOR_FLOW, source_vertex_num, sink_vertex_num);
  
  std::cout << "max flow from " << source_vertex_num
    << " to " << sink_vertex_num
    << ": " << max_flow
    << std::endl;
  assert(max_flow == expected_max_flow);
}

int main()
{
  //TODO: Test a graph that actually needs us to use the residual (reverse) edges to undo.
  test_small(0, 3, 5);
  test_small(0, 2, 5);
  test_small(2, 3, 3);

  return EXIT_SUCCESS;
}


