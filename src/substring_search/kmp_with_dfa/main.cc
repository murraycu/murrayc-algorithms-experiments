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

  // Construct the DFA:
  std::vector<std::vector<std::size_t>> dfa(m + 1, std::vector<std::size_t>(R));

  std::size_t x = 0;
  for (auto i = 0u; i < m; ++i) {
    // Transitions for misses:
    auto& dfai = dfa[i];
    const auto& dfax = dfa[x];
    for (auto r = 0u; r < R; ++r) {
      dfai[r] = dfax[r];
    }

    // Transitions for matches.
    // Finding a matching character takes us to the next position in the
    // pattern:
    const auto pch = pat[i];
    dfai[pch] = i + 1;

    x = dfax[pch];
  }

  // Step over text one character at a time,
  // with no backup,
  // using the DFA to jump around the pattern.
  // We have found a full pattern whenever the DFA takes us to position m.
  std::size_t p = 0;
  for (auto i = 0u; i < n; ++i) {
    const auto ch = str[i];
    p = dfa[p][ch];
    if (p == m) {
      result.emplace_back(i - m + 1);

      // Reset DFA, so we can find overlapping patterns:
      p = dfa[0][ch];
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
  assert(find("findindahaystackneedleina", "needle") ==
         std::vector<std::size_t>({16}));

  return EXIT_SUCCESS;
}
