#include "utils/example_graphs.h"
#include "utils/shortest_path.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <limits>
#include <iostream>
#include <cstdlib>

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;

using type_num = Edge::type_num;
using type_length = Edge::type_length;

using type_shortest_paths = std::vector<type_length>;

const type_length LENGTH_INFINITY = std::numeric_limits<type_length>::max();

using type_map_predecessors = std::vector<type_num>;

/**
 * Calculate the shortest path from @a s to @a v, using at most @i hops (edges).
 */
static
void bellman_ford_update_adjacent_vertex(
  const type_shortest_paths& shortest_paths_i_minus_1, type_shortest_paths& shortest_paths_i,
  type_num i, type_num s, type_num w, const Edge& edge, type_length& shortest_path_so_far,
  type_map_predecessors& predecessors)
{
  const auto v = edge.destination_vertex_;

  //Check the bounds:
  //Otherwise a bad edge in the input can cause an out-of-bounds access:
  if (v >= shortest_paths_i.size())
  {
    std::cerr << "bellman_ford_update_adjacent_vertex(): v is too large: " << v << std::endl;
    return;
  }

  //std::cout << "bellman_ford_update_adjacent_vertex(): i=" << i << ", s=" << s << ", w=" << w << ", v=" << v << std::endl;

  type_length result = LENGTH_INFINITY;
  if(i == 0)
  {
    if(s == v)
    {
      //std::cout << "  i==j: shortest_paths[" << i << "][" << j << "][" << k << "]:" << 0 << std::endl;
      //It takes 0 hops to get from s to v if s and v are the same vertex
      result = 0;
    }
    else
    {
      //There is no way to get from s to v in 0 hops if s is not the same vertex as v:
      result = LENGTH_INFINITY;
    }

    //Cache it:
    shortest_paths_i[v]= result;

    return;
  }

  const type_length case1 = shortest_paths_i_minus_1[v];
  //std::cout << "  case1: existing: shortest_paths[i=" << i-1 << "][" << v << "]:" << case1 << std::endl;

  type_length case2 = LENGTH_INFINITY; //min( w_to_v ) for all w.
  const type_length direct_edge_length = edge.length_;
  const type_length minus_1_s_w = shortest_paths_i_minus_1[w];
  type_length s_to_w_to_v = LENGTH_INFINITY;
  if (direct_edge_length != LENGTH_INFINITY &&
      minus_1_s_w != LENGTH_INFINITY) { //Avoid overflow
    //std::cout << "direct_edge_length: " << direct_edge_length << std::endl;
    //std::cout << "shortest_paths_i_minus_1[w]: " << shortest_paths_i_minus_1[w] << std::endl;
    s_to_w_to_v = minus_1_s_w + direct_edge_length;
    if (s_to_w_to_v < case2) {
      case2 = s_to_w_to_v;
    }
  }

  const auto best = std::min(case1, case2);

  //Make sure that it's less than any cost calculated for the same vertex
  //during this same iteration (i), for another other edge that goes to this
  //same vertex.
  const auto existing = shortest_paths_i[v];
  if (existing < best) {
    result = existing;
  } else {
    result = best;

    //Also update the predecessor, if we've found a new best way to get to this vertex:
    if (case2 < case1) {
      predecessors[v] = w;
    }
  }

  //std::cout << "  result: " << result << std::endl;

  //Cache it:
  //std::cout << "  storing: shortest_paths[i=" << i << "][" << v << "]:" << result << std::endl;
  shortest_paths_i[v] = result;

  if(result < shortest_path_so_far) {
    shortest_path_so_far = result;
  }
  
  //std::cout << "bellman_ford_update_adjacent_vertex: i=" << i << ", s=" << s << ", v=" << v << ", shortest_path_so_far: " << shortest_path_so_far << std::endl;
}

static 
void bellman_ford_single_iteration(const type_vec_nodes& vertices,
  const type_shortest_paths& shortest_paths_i_minus_1, type_shortest_paths& shortest_paths_i,
  type_num i, type_num s, type_length& shortest_path_so_far,
  type_map_predecessors& predecessors)
{
  const auto vertices_count = vertices.size();
  for(type_num v = 0; v < vertices_count; ++v) {
    if (v == s) {
      shortest_paths_i[v] = 0;
    }

    for(const auto& edge : vertices[v].edges_) {
      //bellman_ford_update_adjacent_vertex will use shortest_paths_i_minus_1 and then
      //write new cached values in shortest_paths_i, for use as
      //the minus_1 values for the next call:
      bellman_ford_update_adjacent_vertex(shortest_paths_i_minus_1, shortest_paths_i,
        i, s, v, edge,
        shortest_path_so_far,
        predecessors);
      }
  }
}

/** Get all the shortests paths from s to all other paths.
 * This will be more efficient if the caller firsts removes excess parallel edges,
 * leaving only the lowest-cost edge between each vertex.
 */
static
std::vector<ShortestPath>
bellman_ford_single_source_shortest_path(const type_vec_nodes& vertices, type_num s, bool& has_negative_cycles)
{
  //Initialize output variable:
  has_negative_cycles = false;

  //n:
  const type_num vertices_count = vertices.size();

  type_map_predecessors map_path_predecessor(vertices_count);
  map_path_predecessor[s] = 0; //Invalid

  //Use two 2-D vectors: One for i and one for i-1,
  //just swapping which one we use for i and which one for i-1,
  //to avoid copying and allocating new memory each time we want 
  //to store vector i as vector i-1 and have a new fresh vector i:
  //
  //Note that we could just use one single vertex,
  //instead of always remembering the previous iteration's vertex,
  //but this lets us compare the results to see if there was any
  //change, allowing us to finish early if an iteration doesn't
  //find any shorter paths.
  type_shortest_paths shortest_paths_a(vertices_count);
  type_shortest_paths shortest_paths_b(vertices_count);

  bool i_is_a = true; 
  type_length shortest_path_so_far = LENGTH_INFINITY;

  // i is the number of steps for which we are calculating the shortest path.
  // We need n-1 iterations.
  type_num i = 0;
  for(i = 0; i < (vertices_count - 1); ++i)
  {
    //std::cout << "i=" << i << std::endl;
    auto& shortest_paths_i = i_is_a ?
      shortest_paths_a : shortest_paths_b;
    auto& shortest_paths_i_minus_1 = i_is_a ?
      shortest_paths_b : shortest_paths_a;

    bellman_ford_single_iteration(vertices,
      shortest_paths_i_minus_1, shortest_paths_i,
      i, s, shortest_path_so_far,
      map_path_predecessor);

    //We don't need the previous set of values for i,
    //after we have filled the values for i:
    //Swap around which we use for i, using the other one for i-1;
    i_is_a = !i_is_a;

    //If the shortest paths so far have not changed, then they won't change
    //next time, so we can finish early:
    if (shortest_paths_i == shortest_paths_i_minus_1) {
      //std::cout << "Finishing early at i=" << i <<
      //  ", count=" << vertices_count << std::endl;
      break;
    }
  }

  const auto shortest_path = shortest_path_so_far;

  //Check for a negative cycle:
  //Try one more calculation, this time with i=n.
  //If we get a shorter path then we must have taken a negative cycle:
  auto& shortest_paths_i = i_is_a ?
    shortest_paths_a : shortest_paths_b;
  auto& shortest_paths_i_minus_1 = i_is_a ?
    shortest_paths_b : shortest_paths_a;

  bellman_ford_single_iteration(vertices,
    shortest_paths_i_minus_1, shortest_paths_i,
    i + 1, s, shortest_path_so_far,
    map_path_predecessor);

  if (shortest_path_so_far < shortest_path) {
    has_negative_cycles = true;
  }

  std::vector<ShortestPath> result;
  type_num end_vertex = 0;
  for (const auto& length_and_predecessor : shortest_paths_i) {
    const auto length = length_and_predecessor;

    //Examine the chain of predecessors to get the full path:
    std::string path;
    type_num predecessor = end_vertex;
    while(predecessor != s)
    {
      path = std::to_string(predecessor) + ", " + path;
      predecessor = map_path_predecessor[predecessor];
    }

    path = std::to_string(s) + ", " + path;

    result.emplace_back(ShortestPath(length, path));

    ++end_vertex;
  }
  return result;
}


int main()
{
  const auto start_vertex = 0;
  const auto dest_vertices = {0, 1, 2, 3};

  bool has_negative_cycles = false;
  const auto shortest_paths =
    bellman_ford_single_source_shortest_path(EXAMPLE_GRAPH_SMALL_WITH_NEGATIVE_EDGES,
      start_vertex, has_negative_cycles);

  for(auto dest_vertex_num : dest_vertices)
  {
    const auto shortest_path = shortest_paths[dest_vertex_num];
    std::cout << "shortest path from " << start_vertex << " to " << dest_vertex_num
      << ": " << shortest_path.length_
      << ", path: " << shortest_path.path_ << std::endl;
  }

  return EXIT_SUCCESS;
}


