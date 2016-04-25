#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using uint = unsigned int;
using type_costs = std::vector<uint>;

enum class operations { MATCH, INSERT, DELETE };

uint
match(const char ch_str, const char ch_pattern) {
  if (ch_str == ch_pattern) {
    return 0;
  } else {
    return 1;
  }
}

uint
indel(char /* ch */) {
  return 1;
}

uint
dp_calc_edit_distance(const std::string& str, const std::string& pattern) {
  const auto str_size = str.size();
  const auto pattern_size = pattern.size();

  // We switch between a and b:
  type_costs a(pattern_size, 0);
  type_costs b(pattern_size, 0);
  bool a_is_first = false; // arbitrary.

  for (uint i = 1; i < str_size; ++i) {
    a_is_first = !a_is_first;
    type_costs& costs_i = (a_is_first ? a : b);
    const type_costs& costs_i_minus_1 = (a_is_first ? b : a);

    for (uint j = 1; j < pattern_size; ++j) {
      // Get the cost of the possible operations, and choose the least costly:
      const uint cost_match =
        costs_i_minus_1[j - 1] + match(str[i], pattern[j]);
      const uint cost_insert = costs_i[j - 1] + indel(pattern[j]);
      const uint cost_delete = costs_i_minus_1[j] + indel(str[j]);

      auto min = std::min(cost_match, cost_insert);
      min = std::min(min, cost_delete);
      costs_i[j] = min;
    }

    // costs_i will then be read as costs_i_minus_1;
    // and costs_i_minus_1 will be filled as costs_i;
  }

  type_costs& costs_i = (a_is_first ? a : b);
  return costs_i[pattern_size - 1];
}

int
main() {
  const auto str = "you should not";
  const auto pattern = "though shalt not";

  const auto distance = dp_calc_edit_distance(str, pattern);
  std::cout << "string: " << str << std::endl
            << "pattern: " << pattern << std::endl
            << "distance: " << distance << std::endl;

  return EXIT_SUCCESS;
}
