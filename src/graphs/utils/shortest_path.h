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

  ShortestPath(const ShortestPath& src) = default;
  ShortestPath&
  operator=(const ShortestPath& src) = default;

  ShortestPath(ShortestPath&& src) = default;
  ShortestPath&
  operator=(ShortestPath&& src) = default;

  Edge::type_length length_;
  std::string path_;
};

#endif // MURRAYC_ALGORITHMS_EXPERIMENTS_SHORTEST_PATH
