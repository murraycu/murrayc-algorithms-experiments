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
type_length johnsons_all_pairs_shortest_path(const type_vec_nodes& vertices, bool& has_negative_cycles)
{
  //Initialize ouput variables:
  has_negative_cycles = false;

  if(vertices.empty())
    return Edge::LENGTH_INFINITY;

  //Add an extra vertex s, with zero-weight paths to every existing vertex:
  //There will be no paths into vertex s, so it will not disturb any
  //shortest paths calculations from any other vertex.
  const auto original_size = vertices.size();
  type_vec_nodes vertices_with_s = vertices;
  vertices_with_s.emplace_back();
  const type_num s = original_size;
  auto& vertex_s = vertices_with_s[s];
  for (type_num i = 0; i < original_size; ++i) {
    //Add a zero-weight edge from our extra s vertex to the other vertex:
    vertex_s.edges_.emplace_back(i, 0);
  }

  //Run Bellman-Ford single source shortest path algorithm from our new vertex s to 
  //(We can't use Dijkstra's SSSP algorithm because we can have negative edge lengths).
  //This gives us a value (from s to the vertex) for each vertex:
  const auto shortest_paths_from_s =
    bellman_ford_single_source_shortest_paths(vertices_with_s, s, has_negative_cycles);
  if (has_negative_cycles) {
    std::cerr << "Negative cycle found." << std::endl;
    return Edge::LENGTH_INFINITY;
  }

  //Debug:
  /*
  const auto shortest_paths_from_s_count = shortest_paths_from_s.size();
  for (type_num i = 0; i < shortest_paths_from_s_count; ++i)
  {
    const auto& shortest_path = shortest_paths_from_s[i];
    std::cout << "shortest path from " << s << " to " << i
      << ": " << shortest_path.length_
      << ", path: " << shortest_path.path_ << std::endl;
  }
  */

  //Reweighting:
  //Change the edge lengths based on the discovered values for each vertex
  //(single source shortest paths from our new vertex s):
  //Now they will all be positive.
  auto vertices_reweighted = vertices;
  for (type_num i = 0; i < original_size; ++i) { //Not including the new vertex_s.
    const auto shortest_path_for_i = shortest_paths_from_s[i].length_;
    
    auto& vertex = vertices_reweighted[i];
    for (auto& edge : vertex.edges_) {
      const auto shortest_path_for_dest = shortest_paths_from_s[edge.destination_vertex_].length_;

      //std::cout << "original edge length: " << edge.length_ << std::endl;
      //std:: cout << "  source=" << i << ", dest=" << edge.destination_vertex_ << std::endl;
      //std:: cout << "  shortest_path_for_i =" << shortest_path_for_i <<
      //  ", vertex_dest.shortest_path_for_dest=" <<  shortest_path_for_dest << std::endl;
      const auto diff = shortest_path_for_i - shortest_path_for_dest;
      edge.length_ += diff;
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
  for (type_num u = 0; u < original_size; ++u) {
    const auto shortest_path_u = shortest_paths_from_s[u].length_;

    const auto shortest_paths = dijkstra_compute_shortest_paths(vertices_reweighted, u);
    for (type_num v = 0; v < original_size; ++v) {
      if (u == v)
        continue;
      const auto& shortest_path = shortest_paths[v];
      const auto shortest_path_v = shortest_paths_from_s[v].length_;

      //std::cout << "u=" << u << ", v=" << v << std::endl;
      //std::cout << "  shortest_path=" << shortest_path.length_ << std::endl;

      //Convert to the length of the path in the original set of vertices
      //(with negative edge weights):
      const auto diff = shortest_path_u - shortest_path_v;
      const auto real_shortest_path = shortest_path.length_ - diff;
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


