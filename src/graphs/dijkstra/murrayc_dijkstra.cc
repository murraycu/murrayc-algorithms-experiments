#include <utils/edge.h>
#include "utils/vertex.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>
#include <cstdlib>

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

/**
 * Just a way to return both the length and the path.
 * We could instead use a std::pair or std::tuple.
 */
class ShortestPath
{
public:
  ShortestPath(type_length length, const std::string& path)
  : length_(length),
    path_(path)
  {}

  ShortestPath(const ShortestPath& src) = delete;
  ShortestPath& operator=(const ShortestPath& src) = delete;

  ShortestPath(ShortestPath&& src) = default;
  ShortestPath& operator=(ShortestPath&& src) = default;

  type_length length_;
  std::string path_;
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
    [] (const VertexAndLength& a, const VertexAndLength& b)
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

int main()
{
  //0-indexed vertices:
  const type_vec_nodes vertices = {
    Vertex({Edge(1, 3), Edge(2, 3)}),
    Vertex({Edge(2, 1), Edge(3, 2)}),
    Vertex({Edge(3,50)}),
    Vertex()
  };

  const auto start_vertex = 0;
  const auto dest_vertices = {0, 1, 2, 3};
  for(auto dest_vertex_num : dest_vertices)
  {
    const auto shortest_path = dijkstra_compute_shortest_path(vertices, start_vertex, dest_vertex_num);
    std::cout << "shortest path from " << start_vertex << " to " << dest_vertex_num
      << ": " << shortest_path.length_
      << ", path: " << shortest_path.path_ << std::endl;
  }

  return EXIT_SUCCESS;
}


