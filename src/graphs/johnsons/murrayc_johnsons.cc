#include "bellman_ford/bellman_ford.h"
#include "dijkstra/dijkstra.h"
#include "utils/example_graphs.h"
#include "utils/shortest_path.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cassert>

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;

using type_num = Edge::type_num;
using type_length = Edge::type_length;

static
type_length johnsons_all_pairs_shortest_path(const type_vec_nodes& vertices_in, bool& has_negative_cycles)
{
  auto  vertices = vertices_in;

  //Initialize ouput variables:
  has_negative_cycles = false;

  if(vertices.empty())
    return Edge::LENGTH_INFINITY;

  //const type_num vertices_count = vertices.size();

  //Add an extra vertex s, with zero-weight paths to every existing vertex:
  //There will be no paths into vertex s, so it will not disturb any
  //shortest paths calculations from any other vertex.
  const auto size = vertices.size();
  type_vec_nodes vertices_with_s = vertices;
  vertices_with_s.emplace_back();
  const type_num s = size - 1;
  auto& vertex_s = vertices_with_s[s];
  for (type_num i = 0; i < size; ++i) {
    //Add a zero-weight edge from our extra S vertex to the other vertex:
    vertex_s.edges_.emplace_back(i, 0); 
  }

  //Run Bellman-Ford single source shortest path algorithm from our new vertex s to 
  //(We can't use Dijkstra's SSSP algorithm because we can have negative edge lengths).
  //This gives us a value (from s to the vertex) for each vertex:
  const auto shortest_paths =
    bellman_ford_single_source_shortest_paths(vertices_with_s, s, has_negative_cycles);
  if (has_negative_cycles) {
    std::cerr << "Negative cycle found." << std::endl;
    return Edge::LENGTH_INFINITY;
  }
  
  //The shortest path from the extra vertex s in the reweighting stage
  //of Johnson's APSP algorithm:
  std::unordered_map<type_num, type_length> map_vertex_length_johnsons_sp;

  for (type_num i = 0; i < size; ++i) { //Not including the new vertex_s.
    const auto path_length = shortest_paths[i].length_;
    //std:: cout << "s=" << s << ", i=" << i << ", length_johnsons_sp_: " << path_length << std::endl;
    map_vertex_length_johnsons_sp[i] = path_length;
  }
  
  //Change the edge lengths based on the discovered vertex values
  //(single source shortest paths from our new vertex s):
  //Now they will all be positive.
  for (type_num i = 0; i < size; ++i) { //Not including the new vertex_s.
    const auto vertex_length_johnsons_sp = map_vertex_length_johnsons_sp[i];
    
    auto& vertex = vertices[i];
    for (auto& edge : vertex.edges_) {
      const auto dest_vertex_length_johnsons_sp = map_vertex_length_johnsons_sp[edge.destination_vertex_];

      //std::cout << "original edge length: " << edge.length_ << std::endl;
      //std:: cout << "  source=" << i << ", dest=" << edge.destination_vertex_ << std::endl;
      //std:: cout << "  vertex.length_johnsons_sp_=" << vertex.length_johnsons_sp_ << ", vertex_dest.length_johnsons_sp_=" <<  vertex_dest.length_johnsons_sp_ << std::endl;
      edge.length_ +=
        (vertex_length_johnsons_sp - dest_vertex_length_johnsons_sp);
      //std::cout << "new edge length: " << edge.length_ << std::endl;
      if (edge.length_ < 0) {
        std::cerr << "Error: The new edge length should not be negative: " << edge.length_ << std::endl;
        return Edge::LENGTH_INFINITY;
      }
    }
  }

  //Use Dijkstra's single source shortest path algorithm on each u, v in 
  //the reweighted graph:
  type_length min = Edge::LENGTH_INFINITY;
  for (type_num u = 0; u < size; ++u) {
    for (type_num v = 0; v < size; ++v) {
      if (u == v)
        continue;

      std::cout << "u=" << u << ", v=" << v << std::endl;
      const auto shortest_path = dijkstra_compute_shortest_path(vertices_with_s, u, v);
      
      //Convert to the length of the path in the original set of vertices
      //(with negative edge weights):
      const type_length real_shortest_path = shortest_path.length_
        - map_vertex_length_johnsons_sp[u] + map_vertex_length_johnsons_sp[v];
      if (real_shortest_path < min) {
        min = real_shortest_path;
      }
    }
  }
  
  return min;
}

int main()
{
  //TODO: This implementation seems to be incorrect.
  //It gets different values than the floyd warshall version.
  bool has_negative_cycles = false;
  const auto shortest_path_length =
    johnsons_all_pairs_shortest_path(EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES,
      has_negative_cycles);
  if(has_negative_cycles)
  {
    std::cout << "Negative cycle found." << std::endl;
  }
  else
  {
    std::cout << "Shortest path length: " << shortest_path_length << std::endl;
  }

  assert(shortest_path_length == -10003);

  return EXIT_SUCCESS;
}


