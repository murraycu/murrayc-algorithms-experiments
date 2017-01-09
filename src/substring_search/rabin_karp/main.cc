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

  if (m > n) {
    // The pattern is bigger than the string:
    return {};
  }

  constexpr uint32_t R = 256; // Alphabet size.
  constexpr uint32_t Q = 937; // A large prime.
  uint32_t RM = 1;
  for (auto i = 1u; i < m; ++i) {
    RM = (RM * R) % Q;
  }

  uint32_t pHash = 0;
  uint32_t sHash = 0;
  for (auto i = 0u; i < m; ++i) {
    pHash = (pHash * R + pat[i]) % Q;
    sHash = (sHash * R + str[i]) % Q;
  }

  if (pHash == sHash) {
    result.emplace_back(0);
  }

  for (auto i = m; i < n; ++i) {
    sHash = (sHash + Q - RM * str[i-m] % Q) % Q;
    sHash = (sHash * R + str[i]) % Q;

    if (pHash == sHash) {
      result.emplace_back(i - m + 1);
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

  return EXIT_SUCCESS;
}
