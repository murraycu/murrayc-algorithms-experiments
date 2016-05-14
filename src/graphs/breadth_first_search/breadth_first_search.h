#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_BFS
#define MURRAYC_ALGORITHMS_EXPERIMENTS_BFS

#include "utils/vertex.h"
#include "utils/shortest_path.h"
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <iostream>

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;

using type_num = Edge::type_num;

static
std::vector<type_num>
get_path_from_predecessors(type_num start_vertex, type_num dest_vertex,
  const std::unordered_map<type_num, type_num>& map_path_predecessor)
{
  std::vector<type_num> path;

  type_num predecessor = dest_vertex;
  while(predecessor != start_vertex)
  {
    path.emplace_back(predecessor);

    const auto iter = map_path_predecessor.find(predecessor);
    if (iter == map_path_predecessor.end())
    {
      //This is normal if there is no path to this vertex.
      /*
      std::cerr << "get_path_from_predecessors(): predecessor not found for vertex:"
        << predecessor
        << ", start_vertex=" << start_vertex << ", dest_vertex=" << dest_vertex << std::endl;
      */
      break;
    }

    if (predecessor == iter->second)
    {
      std::cerr << "get_path_from_predecessors(): avoiding infinite loop." << std::endl;
      break;
    }

    predecessor = iter->second;
  }
  
  path.emplace_back(start_vertex);

  std::reverse(path.begin(), path.end());
  return path;
}

std::vector<type_num>
bfs_compute_path(const type_vec_nodes& vertices,
  type_num start_vertex, type_num dest_vertex)
{
  std::vector<type_num> result;

  const auto vertices_size = vertices.size();

  if (start_vertex >= vertices_size) {
    std::cerr << "start vertex not found in vertices: " << start_vertex << std::endl;
    return result;
  }

  if (dest_vertex >= vertices_size) {
    std::cerr << "end vertex not found in vertices: " << dest_vertex << std::endl;
    return result;
  }
  
  std::vector<bool> discovered(vertices_size);
  std::unordered_map<type_num, type_num> predecessors;

  std::queue<type_num> queue;
  queue.emplace(start_vertex);
  discovered[start_vertex] = true;

  bool found = false;

  while(!queue.empty()) {
    const auto i = queue.front();
    queue.pop();
    const auto& vertex = vertices[i];
    
    if (i == dest_vertex) {
      found = true;
      break;
    }
    
    for (const auto& edge : vertex.edges_) {
      const auto edge_dest = edge.destination_vertex_;
      if (discovered[edge_dest])
        continue;
      
      discovered[edge_dest] = true;
      queue.emplace(edge_dest);

      //std::cout << "predecessor of " << edge_dest << " is " << i << std::endl;
      predecessors[edge_dest] = i;
    }
  }

  if (found)
    result = get_path_from_predecessors(start_vertex, dest_vertex, predecessors);

  return result;
}


#endif //MURRAYC_ALGORITHMS_EXPERIMENTS_BFS

