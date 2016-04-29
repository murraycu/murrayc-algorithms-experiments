#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_DIJKSTRA
#define MURRAYC_ALGORITHMS_EXPERIMENTS_DIJKSTRA

#include "utils/vertex.h"
#include "utils/shortest_path.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;

using type_num = Edge::type_num;
using type_length = Edge::type_length;

/**
 * The element for our heap.
 */
class VertexAndLength
{
public:
  VertexAndLength(type_num vertex, type_length total_length, type_num predecessor)
  : vertex_(vertex),
    total_length_(total_length),
    predecessor_(predecessor)
  {}

  VertexAndLength(const VertexAndLength& src) = default;
  VertexAndLength& operator=(const VertexAndLength& src) = default;

  VertexAndLength(VertexAndLength&& src) = default;
  VertexAndLength& operator=(VertexAndLength&& src) = default;

  type_num vertex_;
  type_length total_length_;
  type_num predecessor_;
};

static
ShortestPath dijkstra_compute_shortest_path(const type_vec_nodes& vertices, type_num start_vertex, type_num end_vertex)
{
  const auto vertices_size = vertices.size();

  if (start_vertex >= vertices_size) {
    std::cerr << "start vertex not found in vertices: " << start_vertex << std::endl;
    return ShortestPath(0, "");
  }

  if (end_vertex >= vertices_size) {
    std::cerr << "end vertex not found in vertices: " << end_vertex << std::endl;
    return ShortestPath(0, "");
  }

  //std::cout << " dijkstra_compute_shortest_path(): start_vertex: " << start_vertex << ", end_vertex: " << end_vertex << std::endl;
  //TODO: A simple vector would probably be more efficient,
  //as long as we need to explore almost all the vertices.
  std::unordered_map<type_num, type_length> map_path_lengths;
  std::unordered_map<type_num, type_num> map_path_predecessor;
  map_path_lengths[start_vertex] = 0;
  map_path_predecessor[start_vertex] = 0; //Invalid

  //TODO: A simple vector would probably be more efficient,
  //as long as we need to explore almost all the vertices.
  std::unordered_set<type_num> explored;

  const auto comparator =
    [] (const auto& a, const auto& b)
    {
      return a.total_length_ > b.total_length_;
    };
  std::priority_queue<VertexAndLength, std::vector<VertexAndLength>,
    decltype(comparator)> heap(comparator);
  heap.push(VertexAndLength(start_vertex, 0, 0));

  while(!heap.empty())
  {
    //Get the vertex which has an edge leading to it from the explored set,
    //of minimum length:
    const auto best = heap.top();
    heap.pop();
    const auto best_vertex = best.vertex_;

    if(explored.count(best_vertex) > 0)
    {
      //std::cout << "    best_vertex is in explored." << std::endl;
      //This must be an invalid entry in the heap,
      //which we left to check later, instead of removing.
      //Ignore it.
      continue;
    }

    map_path_lengths[best_vertex] = best.total_length_;
    map_path_predecessor[best_vertex] = best.predecessor_;

    //Stop if we've found the destination vertex.
    //No other path can be shorter,
    //because every part of the path must be the shortest way to get to that part.
    if (best_vertex == end_vertex)
    {
      std::string path;
      type_num predecessor = end_vertex;
      while(predecessor != start_vertex)
      {
        path = std::to_string(predecessor) + ", " + path;
        predecessor = map_path_predecessor[predecessor];
      }

      path = std::to_string(start_vertex) + ", " + path;
      return ShortestPath(best.total_length_, path);
    }

    //Move it to explored:
    explored.emplace(best_vertex);

    const auto& vertex = vertices[best_vertex];
    for(const auto& edge : vertex.edges_)
    {
      const auto edge_destination_vertex = edge.destination_vertex_;

      //Ignore edges leading into the already-explored set:
      if(explored.count(edge_destination_vertex) > 0)
        continue;

      const auto total_length = best.total_length_ + edge.length_;
      heap.push(VertexAndLength(edge_destination_vertex, total_length, best_vertex /* predecessor */));
    }
  }

  return ShortestPath(0, ""); //Shouldn't happen.
}

#endif //MURRAYC_ALGORITHMS_EXPERIMENTS_DIJKSTRA

