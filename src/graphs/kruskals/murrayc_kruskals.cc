#include "utils/edge.h"
#include "utils/vertex.h"
#include "union_find.h"
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>
#include <iostream>
#include <cstdlib>

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;
using type_vec_edges = std::vector<Edge>;

using type_num = Edge::type_num;
using type_length = Edge::type_length;

class EdgeWithSource : public Edge
{
public:
  EdgeWithSource(const Edge& edge, type_num source_vertex)
  : Edge(edge),
    source_vertex_(source_vertex)
  {}
  
  EdgeWithSource(type_num source_vertex, type_num destination_vertex, type_length length)
  : Edge(destination_vertex, length),
    source_vertex_(source_vertex)
  {}

  EdgeWithSource(const EdgeWithSource& src) = default;
  EdgeWithSource& operator=(const EdgeWithSource& src) = default;

  EdgeWithSource(EdgeWithSource&& src) = default;
  EdgeWithSource& operator=(EdgeWithSource&& src) = default;

  type_num source_vertex_;
};

using type_vec_edges_with_sources = std::vector<EdgeWithSource>;

using type_set_msts = std::vector<type_vec_edges_with_sources>;

/**
 * @param max_clusters Call this with 1 to get a single Minimum Spanning Tree.
 */
static
type_set_msts
find_clusters(const type_vec_edges_with_sources& sorted_edges, type_num count_nodes, type_num max_clusters)
{
  type_set_msts result; //Declare this here to try to get some RVO.
  
  //Map of roots to spanning tree costs:
  std::unordered_map<type_num, type_vec_edges_with_sources> map_msts;

  UnionFind<type_num> ds(count_nodes);
  type_num clusters_count = count_nodes;
  type_length min_spacing = std::numeric_limits<type_length>::max();

  for(const auto& edge : sorted_edges) {
    const auto& from = edge.source_vertex_;
    const auto& to = edge.destination_vertex_;
    //std::cout << "from=" << from << ", to=" << to << std::endl;

    const auto from_leader = ds.find_set(from);
    const auto to_leader = ds.find_set(to);

    /*
    if(from_leader == 0) {
      ds.make_set(from);
    }

    if(to_leader == 0) {
      ds.make_set(to);
    }
    */

    //If they were not already in a cluster,
    if(from_leader != to_leader) {
      //std::cout << "clusters_count=" << clusters_count << ", max_clusters=" << max_clusters << std::endl;
      if(clusters_count > max_clusters) {
        //std::cout << "  joining" << std::endl;
        
        //Get and remove existing cost for these clusters/trees:
        //TODO: This would be easier if our UnionFind data structure could
        //store associated data, like a map.
        type_vec_edges_with_sources existing_cost_from;
        auto iter = map_msts.find(ds.find_set(from));
        if (iter != map_msts.end()) {
          existing_cost_from = iter->second;
          map_msts.erase(iter);
        }
        
        type_vec_edges_with_sources existing_cost_to;
        iter = map_msts.find(ds.find_set(to));
        if (iter != map_msts.end()) {
          existing_cost_to = iter->second;
          map_msts.erase(iter);
        }
 
        ds.union_set(from, to);

        //Update the cost for this cluster/tree:
        //Concatenate the list of edges, and the new edge.
        std::copy(existing_cost_to.begin(), existing_cost_to.end(),
          std::back_inserter(existing_cost_from));
        existing_cost_from.emplace_back(edge);
        map_msts[ds.find_set(from)] = existing_cost_from;

        //There is now one less cluster:
        clusters_count--;
      } else {
        //This is a single-item cluster:
        //Add the vertex that is not in a cluster already:
        //This is not a real edge, but it's how we tell the caller
        //that there is only one node in the cluster.
        if (from == from_leader && !map_msts.count(from)) {
          map_msts[from] = {EdgeWithSource(from, from, 0)};
        }

        if(to == to_leader && !map_msts.count(to)) {
          map_msts[to] = {EdgeWithSource(to, to, 0)};
        }

        //std::cout << "  remembering max distance" << std::endl;
        //Remember the biggest spacing between remaining clusters:
        if(edge.length_ < min_spacing) {
          min_spacing = edge.length_;
        }
      }
    } else {
      //These edges are already joined.
      //std::cout << "  already joined:" << std::endl;
    }
  }

  //std::cout << "min_spacing: " << min_spacing << std::endl;
  
  //Return only the costs of the clusters/trees,
  //not including the UnionFind's roots:
  std::transform(map_msts.begin(), map_msts.end(),
    std::inserter(result, result.end()),
    [](const auto& a) {
      return a.second;
    });
  return result;
}

static
type_set_msts compute_mst_cost(const type_vec_nodes& vertices)
{
  type_vec_edges_with_sources edges;
  const auto vertices_count = vertices.size();
  for (type_num v = 0; v < vertices_count; ++v) {
    const auto& vertex_edges = vertices[v].edges_;

    std::transform(vertex_edges.begin(), vertex_edges.end(),
      std::back_inserter(edges),
      [v](const auto& edge) {
        return EdgeWithSource(edge, v);
      });
  }

  //Sort the edges by their distance (ascending):
  std::sort(edges.begin(), edges.end(),
    [](const auto& a, const auto& b) -> bool {
      return a.length_ < b.length_;
    });

  return find_clusters(edges, vertices.size(), 2);
}

int main()
{
  //0-indexed vertices:
  const type_vec_nodes vertices = {
    Vertex({Edge(1, 3), Edge(2, 3)}),
    Vertex({Edge(2, 1), Edge(3, 2)}),
    Vertex({Edge(3, 50)}),
    Vertex()
  };

  const auto msts = compute_mst_cost(vertices);
  if (msts.size() == 1) {
    std::cout << "There is a minimum spanning tree:" << std::endl;
  } else {
    std::cout << "There is not a single minimum spanning tree:" << std::endl;
  }

  for (const auto& mst : msts) {
    type_length cost = 0;
    for (const auto& edge : mst) {
      std::cout << "edge: " << edge.source_vertex_ << " to " << edge.destination_vertex_
        << ", cost=" << edge.length_ << std::endl;
      cost += edge.length_;
    }

    std::cout << "  MST cost: " << cost << std::endl;
  }

  return EXIT_SUCCESS;
}


