#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>

using uint = unsigned int;
using type_costs = std::vector<std::vector<uint>> ;

enum class operations {
  MATCH,
  INSERT,
  DELETE
};

uint match(const char ch_str, const char ch_pattern) {
  if (ch_str == ch_pattern) {
    return 0;
  } else {
    return 1;
  }
}

uint indel(char /* ch */) {
  return 1;
}

uint dp_calc_edit_distance(const std::string& str, const std::string& pattern) {
  const auto str_size = str.size();
  const auto pattern_size = pattern.size();
  
  type_costs m(str_size);
  for(auto& sub : m) {
    sub.resize(pattern_size, 0);
  }

  for (uint i = 1; i < str_size; ++i) {
    for (uint j = 1; j < pattern_size; ++j) {
      //Get the cost of the possible operations, and choose the least costly:
      const uint cost_match = m[i-1][j-1] + match(str[i], pattern[j]);
      const uint cost_insert = m[i][j-1] + indel(pattern[j]);
      const uint cost_delete = m[i-1][j] + indel(str[j]);
      
      auto min = std::min(cost_match, cost_insert);
      min = std::min(min, cost_delete);
      m[i][j] = min;
    }
  }
  
  return m[str_size-1][pattern_size-1];
}

int main() {
  const auto str = "you should not";
  const auto pattern = "though shalt not";
  
  const auto distance = dp_calc_edit_distance(str, pattern);
  std::cout << "string: " << str << std::endl
    << "pattern: " << pattern << std::endl
    << "distance: " << distance << std::endl;
  
  return EXIT_SUCCESS;
}
