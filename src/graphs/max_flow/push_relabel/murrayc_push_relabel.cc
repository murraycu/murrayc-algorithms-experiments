#include "utils/example_graphs.h"
#include "shortest_path/breadth_first_search/breadth_first_search.h"
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

bool has_outgoing_downhill_capacity(const type_vec_nodes& vertices,
  const type_num vertex_num, const std::vector<Edge::type_length>& heights)
{
  const auto vertex_height = heights[vertex_num];
  const auto& vertex = vertices[vertex_num];

  for (const auto& edge : vertex.edges_) {
    if (edge.length_ > 0 && vertex_height > heights[edge.destination_vertex_])
      return true;
  }

  return false;
}

static
Edge::type_length
push_relabel_max_flow(const type_vec_nodes& vertices, type_num source_vertex_num, type_num sink_vertex_num)
{
  //TODO: Separate map of edges to their flows?
  auto residual_graph = make_residual_graph(vertices);

  const auto vertices_count = vertices.size();
  std::vector<Edge::type_length> heights(vertices_count);
  heights[source_vertex_num] = vertices_count;
  std::vector<Edge::type_length> excesses(vertices_count);

  // This should return true if a is less than b.
  const auto comparator =
    [&residual_graph, &heights, &excesses, sink_vertex_num] (const auto& a, const auto& b)
    {
      // If they are the same index, then a is not less than b.
      if (a == b) {
        return false;
      }

      const auto excess_a = excesses[a];
      const auto excess_b = excesses[b];

      // If only one has excess,the one with excess comes first.
      if (excess_a > 0 && excess_b <= 0) {
        return true;
      } else if (excess_b > 0 && excess_a <= 0) {
        return false;
      }

      // Both a and b have have excess:

      //if (excess_a != excess_b) {
      //  return excess_a > excess_b;
      //}

      //Make sure that the sink vertex is always the last one,
      if (a == sink_vertex_num) {
        return false;
      } else if (b == sink_vertex_num) {
        return false;
      }

      const auto height_a = heights[a];
      const auto height_b = heights[b];
      if (height_a == height_b) {
        return excess_a > excess_b; //TODO: Alternative ways of choosing?
      }

      return height_a > height_b;
    };

  //We don't use a std::priority_queue() because we want to manually
  //reorder the items after external changes.
  //std::priority_queue<type_num, std::vector<type_num>,
  //  decltype(comparator)> heap(comparator);
  std::vector<type_num> heap(vertices_count);
  for (type_num i = 0; i < vertices_count; ++i) {
    heap[i] = i;
  }
  std::make_heap(heap.begin(), heap.end(), comparator);

  const auto predicate_edge_has_capacity =
    [](const Edge& edge) {
      return edge.length_ > 0;
    };

  type_num best = source_vertex_num;
  while(excesses[best] > 0) {
    //If there is a higher vertex with outgoing capacity,
    //send flow along that edge.
    if (has_outgoing_downhill_capacity(
          residual_graph, best, heights)) {
      auto& excess = excesses[best];

      auto& vertex = residual_graph[best];
      auto& edges = vertex.edges_;
      auto iter_edge = std::find_if(edges.begin(), edges.end(),
        predicate_edge_has_capacity);
      while (excess > 0 && iter_edge != edges.end()) {
        //Reduce the edge's capacity:
        //std::cout << "before:"
        //  << ", capacity=" << edge.length_
        //  << ", residual=" << edge.residual_ << std::endl;
        auto& edge = *iter_edge;
        const auto c = std::min(edge.length_, excess);
        edge.length_ -= c;
        //std::cout << "after: edge.length_=" << edge.length_ << std::endl;

        //Increase the reverse edge's capacity, to allow an undo:
        auto& reverse_edge = get_reverse_edge(edge, residual_graph);
        reverse_edge.length_ += c;

        //Move the excess from the vertex to its destination:
        excess -= c;

        auto& excess_dest = excesses[edge.destination_vertex_];
        excess_dest +=c;

        iter_edge = std::find_if(iter_edge, edges.end(),
          predicate_edge_has_capacity);
      }
    } else if (best == sink_vertex_num) {
      std::cerr << "Unexpectedly trying to relabel destination vertex.\n";
    } else {
      // Relabel the highest vertex and try again.
      heights[best]++;
    }

    // Get the new best vertex,
    // (Of the vertices with excess, the highest one.)
    std::make_heap(heap.begin(), heap.end(), comparator);
    best = heap.front();
  }

  Edge::type_length result = 0;
  const auto& source_vertex = residual_graph[source_vertex_num];
  for (const auto& edge : source_vertex.edges_) {
    result += edge.length_;
  }

  return result;
}

static
void test_small(type_num source_vertex_num, type_num sink_vertex_num, Edge::type_length expected_max_flow)
{
  const auto max_flow = push_relabel_max_flow(EXAMPLE_GRAPH_SMALL_FOR_FLOW, source_vertex_num, sink_vertex_num);

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


