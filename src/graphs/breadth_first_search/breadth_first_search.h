#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_BFS
#define MURRAYC_ALGORITHMS_EXPERIMENTS_BFS

#include "utils/source_and_edge.h"
#include "utils/shortest_path.h"
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <iostream>

using type_num = Edge::type_num;

static
type_vec_path
get_path_from_predecessors(type_num start_vertex, type_num dest_vertex,
  const std::unordered_map<type_num, SourceAndEdge>& map_path_predecessor)
{
  type_vec_path path;

  type_num predecessor = dest_vertex;
  while(predecessor != start_vertex)
  {
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

    path.emplace_back(iter->second);

    if (predecessor == iter->second.edge_)
    {
      std::cerr << "get_path_from_predecessors(): avoiding infinite loop." << std::endl;
      break;
    }

    predecessor = iter->second.source_;
  }
  
  //path.emplace_back(SourceAndEdge(start_vertex, 0));

  std::reverse(path.begin(), path.end());
  return path;
}

bool
bfs_compute_path(const type_vec_nodes& vertices,
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

      //std::cout << "predecessor of " << edge_dest << " is " << i << std::endl;
      predecessors[edge_dest] = SourceAndEdge(i, e);
    }
  }

  if (found)
    result = get_path_from_predecessors(start_vertex, dest_vertex, predecessors);

  path = result;
  return found;
}


#endif //MURRAYC_ALGORITHMS_EXPERIMENTS_BFS

