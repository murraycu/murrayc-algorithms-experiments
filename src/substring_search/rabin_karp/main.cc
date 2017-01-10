#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

static inline uint32_t
hash(const std::string& str, std::size_t len, uint32_t R, uint32_t Q) {
  uint32_t h = 0;
  for (auto i = 0u; i < len; ++i) {
    h = (h * R + str[i]) % Q;
  }

  return h;
}

static inline uint32_t
hash(const std::string& str, uint32_t R, uint32_t Q) {
  return hash(str, str.size(), R, Q);
}

static inline bool
check(const std::string& str, std::size_t start, const std::string& pat) {
  auto i = std::cbegin(str) + start;
  for (auto ch : pat) {
    if (ch != *i) {
      return false;
    }

    ++i;
  }

  return true;
}

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

  const auto pHash = hash(pat, R, Q);
  auto sHash = hash(str, m, R, Q);

  if (pHash == sHash && check(str, 0, pat)) {
    result.emplace_back(0);
  }

  uint32_t RM = 1;
  for (auto i = 1u; i < m; ++i) {
    RM = (RM * R) % Q;
  }

  for (auto i = m; i < n; ++i) {
    sHash = (sHash + Q - RM * str[i-m] % Q) % Q;
    sHash = (sHash * R + str[i]) % Q;

    if (pHash == sHash) {
      const auto pos = i - m + 1;
      if (check(str, pos, pat)) {
        result.emplace_back(pos);
      }
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
