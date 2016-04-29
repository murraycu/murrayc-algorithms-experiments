#include "utils/edge.h"
#include "utils/vertex.h"
#include "utils/shortest_path.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <limits>
#include <algorithm>
#include <iostream>
#include <cstdlib>

// A set of vertices and their edges.
using type_vec_nodes = std::vector<Vertex>;

using type_num = Edge::type_num;
using type_length = Edge::type_length;

const type_length LENGTH_INFINITY = std::numeric_limits<type_length>::max();




static
type_length get_direct_edge_length(const type_vec_nodes& vertices, type_num i, type_num j)
{
  //std::cout << "get_direct_edge_length() i=" << i << ", j=" << j << std::endl;

  const auto& vertex = vertices[i];
  const auto& edges = vertex.edges_;
  const auto iter = std::find_if(edges.begin(), edges.end(),
    [j](const auto& edge) {
       return edge.destination_vertex_ == j;
    });
  if(iter == edges.end())
    return LENGTH_INFINITY;
  else
    return iter->length_;
}

/*
template<typename T>
void print_vec(const std::vector<T>& vec)
{
  for(auto num : vec)
  {
    std::cout << num << ", ";
  }
}
*/

using type_shortest_paths = std::vector<std::vector<type_length>> ;

static
void calc_with_cache(const type_vec_nodes& vertices, type_shortest_paths& shortest_paths_k, type_shortest_paths& shortest_paths_k_minus_1,
  type_num i, type_num j, type_length k, type_length& shortest_path_so_far, bool& has_negative_cycles)
{
  //std::cout << "calc_with_cache(): i=" << i << ", j=" << j << ", k=" << k << std::endl;

  type_length result = LENGTH_INFINITY;
  if(k == 0)
  {
    if(i == j)
    {
      //std::cout << "  i==j: shortest_paths[" << i << "][" << j << "][" << k << "]:" << 0 << std::endl;
      result = 0;
    }
    else
    {
      //std::cout << "  k==0: shortest_paths[" << i << "][" << j << "][" << k << "]:" << 0 << std::endl;
      result = get_direct_edge_length(vertices, i, j);
    }

    //Cache it:
    shortest_paths_k[i][j] = result;
    //std::cout << "  base: " << result << std::endl;
    return;
  }

  const type_length case1 = shortest_paths_k_minus_1[i][j];
  if(has_negative_cycles)
    return;

  //std::cout << "  case1: shortest_paths[" << i << "][" << j << "][" << k-1 << "]:" << case1 << std::endl;

  //Avoid adding infinity to infinity, which would overflow.
  type_length case2 = LENGTH_INFINITY;
  //std::cout << "  i_to_k: [" << i << "][" << k << "][" << k - 1 << "]" << std::endl;
  const auto i_to_k = shortest_paths_k_minus_1[i][k];
  //std::cout << "    i_to_k: shortest_paths[" << i << "][" << k << "][" << k - 1 << "]:" << i_to_k << std::endl;

  //std::cout << "  k_to_j: [" << k << "][" << j << "][" << k - 1 << "]" << std::endl;
  const auto k_to_j = shortest_paths_k_minus_1[k][j];
  //std::cout << "    k_to_j: shortest_paths[" << k << "][" << j << "][" << k - 1 << "]:" << k_to_j << std::endl;

  if(i_to_k != LENGTH_INFINITY && k_to_j != LENGTH_INFINITY)
  {
    case2 = i_to_k + k_to_j;
  }

  result = std::min(case1, case2);

  //Cache it:
  //std::cout << "k=" << k << std::endl;
  shortest_paths_k[i][j] = result;

  //Detect negative cycle,
  //when k!=0 and i==j,
  //meaning that there are some paths that are length less than the 0 it
  //would take to just stay at the node itself.
  if(i == j && result < 0) {
    has_negative_cycles = true;
    return;
  }

  if(result < shortest_path_so_far) {
    shortest_path_so_far = result;
  }
}

void resize_shortest_paths(type_shortest_paths& shortest_paths_k, type_num vertices_count)
{
  for(auto& vec_j : shortest_paths_k)
  {
    vec_j.resize(vertices_count + 1); //1-indexed.
  }
}

void wipe_shortest_paths(type_shortest_paths& shortest_paths_k)
{
  for(auto& vec_j : shortest_paths_k)
  {
    std::fill(vec_j.begin(), vec_j.end(), 0);
  }
}

static
type_length floyd_warshall_calc_all_pairs_shortest_path(const type_vec_nodes& vertices, bool& has_negative_cycles)
{
  type_length shortest_path_so_far = LENGTH_INFINITY;

  //Initialize ouput variables:
  has_negative_cycles = false;

  if(vertices.empty())
    return LENGTH_INFINITY;

  const type_num vertices_count = vertices.size();

  //[node i, node j, using path of k nodes]

  //Use two 2-D vectors: One for k and one for k-1,
  //just swapping which one we use for k and which one for k-1,
  //to avoid copying and allocating new memory each time we want 
  //to store vector k as vector k-1 and have a new fresh vector k:
  type_shortest_paths shortest_paths_a(vertices_count + 1);
  type_shortest_paths shortest_paths_b(vertices_count + 1);
  resize_shortest_paths(shortest_paths_a, vertices_count + 1);
  resize_shortest_paths(shortest_paths_b, vertices_count + 1);

  bool k_is_a = true;
  for(type_num k = 0; k <= vertices_count; ++k)
  {
    type_shortest_paths& shortest_paths_k = k_is_a ?
      shortest_paths_a : shortest_paths_b;
    type_shortest_paths& shortest_paths_k_minus_1 = k_is_a ?
      shortest_paths_b : shortest_paths_a;

    if(k > 1)
      wipe_shortest_paths(shortest_paths_k);

    for(type_num i = 0; i < vertices_count; ++i)
    {
      // TODO: Make this work, without doing a recursive version.
      //const type_num max_j = (check_negative_cycle_only ?
      //  std::max(i, k) :
      //  vertices_count);
      for(type_num j = 0; j < vertices_count; ++j)
      {
        calc_with_cache(vertices, shortest_paths_k, shortest_paths_k_minus_1,
          i, j, k,
          shortest_path_so_far, has_negative_cycles);

        //Abort as soon as we see a negative cycle
        //(and elsewhere),
        //because the Floyd Warshall algorithm cannot handle a negative cycle.
        if(has_negative_cycles) {
          return 0;
        }
      }
    }

    if(k != 0 && k % 200 == 0)
    {
      std::cout << "k done: " << k << std::endl;
    }

    //We don't need the previous set of values for k,
    //after we have filled the values for k:
    //Swap around which we use for k, using the other one for k-1;
    k_is_a = !k_is_a;
  }

  return shortest_path_so_far;
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

  bool has_negative_cycles = false;
  const auto shortest_path_length =
    floyd_warshall_calc_all_pairs_shortest_path(vertices, has_negative_cycles);
  if(has_negative_cycles)
  {
    std::cout << "Negative cycle found." << std::endl;
  }
  else
  {
    std::cout << "Shortest path length: " << shortest_path_length << std::endl;
  }

  return EXIT_SUCCESS;
}


