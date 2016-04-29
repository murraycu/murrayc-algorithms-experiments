#include "utils/example_graphs.h"
#include <unordered_set>
#include <queue>
#include <limits>
#include <iostream>
#include <cstdlib>

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;

using type_num = Edge::type_num;
using type_length = Edge::type_length;

static
type_length
compute_mst_cost(const type_vec_nodes& nodes)
{
  typedef std::unordered_set<type_num> type_set_nodes;
  type_set_nodes unexplored;
  const auto nodes_size = nodes.size();
  for(type_num i = 0; i < nodes_size; ++i)
  {
    unexplored.emplace(i);
  }

  type_set_nodes mst_nodes;
  std::vector<Edge> mst_edges;

  //Move start_node from the unexplored set to the explored set:
  //const auto start_node_iter = unexplored.begin();  //Arbitrary. It shouldn't matter.
  const auto start_node_num = 0; //*start_node_iter;
  //std::cout << "start_node: " << start_node_num << std::endl;
  mst_nodes.emplace(start_node_num);
  unexplored.erase(start_node_num);

  constexpr auto MAX_COST = std::numeric_limits<type_length>::max();
  constexpr auto MAX_NODE_NUM = std::numeric_limits<type_num>::max();

  while(!unexplored.empty())
  {
    //std::cout << "loop: mst_nodes.size() = " << mst_nodes.size() << ", unexplored.size()=" << unexplored.size() << std::endl;
    //Find the outgoing edge from the current tree's nodes with the smallest cost:
    type_length min_cost = MAX_COST;
    type_num min_cost_node = MAX_NODE_NUM;
    Edge min_cost_edge;

    for(const auto& mst_node_num : mst_nodes) {
      const auto& mst_node = nodes[mst_node_num];
      //std::cout << "checking mst_nodes: mst_node_num=" << mst_node_num << ", mst_node.edges_.size()=" << mst_node.edges_.size() << std::endl;

      //Discard edges with no nodes,
      //though that would be an error in the input file:
      if(mst_node.edges_.empty()) {
        std::cerr << "Ignoring node with no edge: node num=" << mst_node_num << std::endl;
        continue;
      }

      for(const auto& edge : mst_node.edges_) {
        //Ignore edges that don't lead out of the current tree:
        //std::cout << "Checking if destination_node is in mst_nodes: " << edge.destination_vertex_ << std::endl;
        if(mst_nodes.count(edge.destination_vertex_)) {
          continue;
        }

        if(edge.length_ < min_cost) {
          min_cost = edge.length_;
          min_cost_node = edge.destination_vertex_;
          min_cost_edge = edge; //TODO: Inefficient, because we discard most of these.
        }
      }
    }

    if(min_cost_node != MAX_NODE_NUM) {
      //std::cout << "min cost node num: " << min_cost_node << ", edge cost: " << min_cost_edge.length_ <<  std::endl;
      mst_nodes.emplace(min_cost_node);
      mst_edges.emplace_back(min_cost_edge);

      //Remove it from the unexplored set:
      unexplored.erase(min_cost_node);
    }
  }

  //Calculate the complete cost of the edges:
  type_length cost = 0;
  for(const auto& edge: mst_edges) {
    cost += edge.length_;
  }

  return cost;
}

int main()
{
  const auto cost = compute_mst_cost(EXAMPLE_GRAPH_SMALL);
  std::cout << "MST cost: " << cost << std::endl;

  return EXIT_SUCCESS;
}


