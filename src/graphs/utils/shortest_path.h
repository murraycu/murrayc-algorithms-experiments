#ifndef MURRAYC_ALGORITHMS_EXPERIMENTS_SHORTEST_PATH
#define MURRAYC_ALGORITHMS_EXPERIMENTS_SHORTEST_PATH

#include "utils/edge.h"
#include <string>

/**
 * Just a way to return both the length and the path.
 * We could instead use a std::pair or std::tuple.
 */
class ShortestPath {
public:
  ShortestPath(Edge::type_length length, const std::string& path)
  : length_(length), path_(path) {}

  Edge::type_length length_;
  std::string path_;
};

static_assert(std::is_copy_assignable<ShortestPath>::value,
  "ShortestPath should be copy assignable.");
static_assert(std::is_copy_constructible<ShortestPath>::value,
  "ShortestPath should be copy constructible.");
static_assert(std::is_move_assignable<ShortestPath>::value,
  "ShortestPath should be move assignable.");
static_assert(std::is_move_constructible<ShortestPath>::value,
  "ShortestPath should be move constructible.");
#endif // MURRAYC_ALGORITHMS_EXPERIMENTS_SHORTEST_PATH
