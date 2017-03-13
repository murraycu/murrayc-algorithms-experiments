#include <cstdlib>
#include <cassert>
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

  // 0 means empty string.
  // and 1 is just the first character of the string.

  // We switch between a and b:
  type_costs a(pattern_size + 1, 0);
  type_costs b(pattern_size + 1, 0);
  bool a_is_first = false; // arbitrary.

  for (uint i = 0; i <= str_size; ++i) {
    a_is_first = !a_is_first;
    type_costs& costs_i = (a_is_first ? a : b);
    const type_costs& costs_i_minus_1 = (a_is_first ? b : a);

    for (uint j = 0; j <= pattern_size; ++j) {
      // 0 means empty string, so converting an empty to non-empty, or vice-versa,
      // means adding or deleting all the characters
      if (i == 0) {
        // Add all the characters from B:
        costs_i[j] = j;
        continue;
      } else if (j == 0) {
        // Delete all the characters from A:
        costs_i[j] = i;
        continue;
      }

      // Get the cost of the possible operations, and choose the least costly:

      // We subtract 1 when comparing characters, because i and j are 1-indexed,
      // with 0 meaning an empty string.
      const uint cost_match =
        costs_i_minus_1[j - 1] + match(str[i - 1], pattern[j - 1]);

      const uint cost_insert = costs_i[j - 1] + indel(pattern[j]);
      const uint cost_delete = costs_i_minus_1[j] + indel(str[j]);

      auto min = std::min(cost_match, cost_insert);
      min = std::min(min, cost_delete);
      costs_i[j] = min;
    }

    // costs_i will then be read as costs_i_minus_1;
    // and costs_i_minus_1 will be filled as costs_i;
  }

  const type_costs& costs_i = (a_is_first ? a : b);
  return costs_i[pattern_size];
}

int
main() {
  const auto str = "you should not";
  const auto pattern = "though shalt not";

  const auto distance = dp_calc_edit_distance(str, pattern);
  std::cout << "string: " << str << std::endl
            << "pattern: " << pattern << std::endl
            << "distance: " << distance << std::endl;

  assert(dp_calc_edit_distance("you should not", "though shalt not") == 7);

  assert(dp_calc_edit_distance("", "") == 0);
  assert(dp_calc_edit_distance("a", "") == 1);
  assert(dp_calc_edit_distance("", "a") == 1);
  assert(dp_calc_edit_distance("a", "b") == 1);
  assert(dp_calc_edit_distance("aaa", "aa") == 1);
  assert(dp_calc_edit_distance("aa", "aaa") == 1);
  assert(dp_calc_edit_distance("abac", "aac") == 1);

  return EXIT_SUCCESS;
}
