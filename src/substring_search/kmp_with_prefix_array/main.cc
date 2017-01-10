#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

static inline std::vector<std::size_t>
construct_prefix_array(const std::string& pat) {
  const auto m = pat.size();
  std::vector<std::size_t> pa(m);

  std::size_t i = 1;
  std::size_t x = 0;
  while (i < m ) {
    const auto chi = pat[i];
    const auto chx = pat[x];

    if (chi == chx) {
      // The size of the suffix that matches the prefix:
      pa[i] = x + 1;
      ++i;
      ++x;
    } else {
      // Backtrack to previous matching prefix, if any:
      if (x == 0) {
        pa[i] = 0;
        ++i;
      } else {
        x = pa[x - 1];
      }
    }
  }

  return pa;
}

static std::vector<std::size_t>
find(const std::string& str, const std::string& pat) {
  std::vector<std::size_t> result;
  const auto m = pat.size();
  const auto n = str.size();

  if (m == 0) {
    return {};
  }

  if (m > n) {
    // The pattern is bigger than the string:
    return {};
  }

  const auto pa = construct_prefix_array(pat);

  // Step over text one character at a time,
  // with no backup,
  // using the pattern array to jump around the pattern.
  // We have found a full pattern whenever the pattern array takes us to position m.
  std::size_t p = 0;
  for (auto i = 0u; i < n; ++i) {
    const auto ch = str[i];
    const auto chp = pat[p];
    if (ch == chp) {
      ++p;
    } else {
      if (p != 0) {
        p = pa[p - 1];
      }
    }

    if (p == m) {
      result.emplace_back(i - m + 1);
      p = pa[p - 1];
    }
  }

  return result;
}

int
main() {
  // Test the prefix array construction:
  // See https://youtu.be/KG44VoDtsAA
  const std::vector<std::size_t> expected = {0, 0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 4};
  const auto pa = construct_prefix_array("acacabacacabacacac");
  /*
  for (auto i : pa) {
    std::cout << i << ", ";
  }
  std::cout << std::endl;
  */
  assert(pa == expected);

  // Test substring search:
  assert(find("foo", "fo") == std::vector<std::size_t>({0}));
  assert(find("fop", "p") == std::vector<std::size_t>({2}));
  assert(find("xabcabzabc", "abc") == std::vector<std::size_t>({1, 7}));
  assert(find("foobarmoo", "oob") == std::vector<std::size_t>({1}));
  assert(find("foobarmoo", "bar") == std::vector<std::size_t>({3}));

  // Overlapping:
  assert(find("ababababa", "aba") == std::vector<std::size_t>({0, 2, 4, 6}));

  // Longer:
  assert(find("findindahaystackneedleina", "needle") == std::vector<std::size_t>({16}));

  return EXIT_SUCCESS;
}
