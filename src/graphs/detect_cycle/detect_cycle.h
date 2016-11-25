#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_DETECT_CYCLE
#define MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_DETECT_CYCLE

#include "utils/vertex.h"
#include "utils/example_graphs.h"
#include <vector>
#include <stack>
#include <iostream>

bool
detect_cycle_recursive(const type_vec_nodes& vertices,
  Edge::type_num s, std::vector<bool>& discovered, std::vector<bool>& completed) {
  discovered[s] = true;

  const auto& v = vertices[s];
  for (const auto& e : v.edges_) {
    const auto d = e.destination_vertex_;
    if (completed[d]) {
      continue;
    }

    // We are already exploring this vertex,
    // and that exploration has not completed:
    if (discovered[d]) {
      // cycle found.
      return true;
    }

    if (detect_cycle_recursive(vertices, d, discovered, completed)) {
      return true;
    }
  }

  completed[s] = true;

  return false;
}


/**
 * DFS to discover any cycle starting from vertex @a s.
 */
bool
detect_cycle_recursive(const type_vec_nodes& vertices,
  Edge::type_num s) {
  const auto n = vertices.size();

  // DFS on the tree to find a cycle.
  std::vector<bool> discovered(n);
  std::vector<bool> completed(n);
  return detect_cycle_recursive(vertices, s, discovered, completed);
}

/**
 * DFS to discover any cycle.
 */
bool
detect_cycle_recursive(const type_vec_nodes& vertices) {
  const auto n = vertices.size();
  std::vector<bool> discovered(n);
  std::vector<bool> completed(n);
  for (std::size_t i = 0; i < n; ++i) {
    if (detect_cycle_recursive(vertices, i, discovered, completed)) {
      return true;
    }
  }

  return false;
}

/**
 * DFS to discover any cycle.
 */
bool
detect_cycle_iterative(const type_vec_nodes& vertices,
  Edge::type_num s, std::vector<bool>& discovered, std::vector<bool>& completed) {

  // DFS on the tree to find a cycle.

  // A stack of <vertex index, is-completion>:
  // to match the implicit behaviour of the recursive version,
  // which sets the completion flag for the vertex after the recursive calls.
  std::stack<std::pair<Edge::type_num, bool>> st;
  st.emplace(s, false);
  discovered[s] = true;
  bool cycle_found = false;
  while (!st.empty()) {
    const auto& p = st.top();
    st.pop();
    const auto vnum = p.first;
    const auto is_a_completion = p.second;
    if (is_a_completion) {
      completed[vnum] = true;
      continue;
    }

    discovered[vnum] = true;

    const auto& v = vertices[vnum];
    for (auto e : v.edges_) {
      const auto d = e.destination_vertex_;
      if (completed[d]) {
        continue;
      }

      // We are already exploring this vertex,
      // and that exploration has not completed:
      if (discovered[d]) {
        // cycle found.
        return true;
      }

      st.emplace(d, false);
    }

    // Make sure that we pull a completion for this node off
    // the stack after we pull its child nodes (and their completions).
    st.emplace(vnum, true);
  }

  return cycle_found;
}

/**
 * DFS to discover any cycle starting from vertex @a s.
 */
bool
detect_cycle_iterative(const type_vec_nodes& vertices,
  Edge::type_num s) {
  const auto n = vertices.size();

  // DFS on the tree to find a cycle.
  std::vector<bool> discovered(n);
  std::vector<bool> completed(n);
  return detect_cycle_iterative(vertices, s, discovered, completed);
}

/**
 * DFS to discover any cycle.
 */
bool
detect_cycle_iterative(const type_vec_nodes& vertices) {
  const auto n = vertices.size();
  std::vector<bool> discovered(n);
  std::vector<bool> completed(n);
  for (std::size_t i = 0; i < n; ++i) {
    if (detect_cycle_iterative(vertices, i, discovered, completed)) {
      return true;
    }
  }

  return false;
}

/**
 * DFS to discover any cycle starting from vertex @a s.
 */
bool
detect_cycle(const type_vec_nodes& vertices,
  Edge::type_num s) {
  return detect_cycle_recursive(vertices, s);
}

#endif // MURRAYC_ALGORITHMS_EXPERIMENTS_GRAPHS_DETECT_CYCLE
