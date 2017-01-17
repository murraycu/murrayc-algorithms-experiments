#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

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

  constexpr uint32_t R = 256; // Alphabet size.

  // Discover rightmost positions of characters in pattern:
  std::vector<int32_t> r(R, -1);
  for (auto i = 0u; i < m; ++i) {
    const auto ch = pat[i];
    r[ch] = i;
  }

  for (auto i = m - 1; i < n; ++i) {
    std::size_t j = 0u;
    bool found = true;
    for (j = 0u; j < m; ++j) {
      const auto p = m-j-1;
      const auto pch = pat[p];
      const auto ch = str[i-j];
      if (pch != ch) {
        found = false;
        const auto rightmost = r[ch];
        if (rightmost == -1 && p > 0) {
          // The character is nowhere in the pattern,
          // so skip completely past this character.
          i += p - 1; //We always increment by 1 anyway.
        } else if (rightmost > 0 && (uint32_t)(rightmost) < p) {
          const auto skip = p - rightmost;
          i += skip - 1; // We always increment by 1 anyway.
        }
        break;
      }
    }

    if (found) {
      result.emplace_back(i-m+1);
    }
  }

  return result;
}

int
main() {
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
