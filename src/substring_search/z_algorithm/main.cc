#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

static std::vector<std::size_t>
make_z(const std::string& s) {
  const auto n = s.size();
  std::vector<std::size_t> z(n);
  std::size_t l = 0;
  std::size_t r = 0;
  for (std::size_t i = 1; i < n; ++i) {
    // If we are inside a previously-discovered z box:
    if (i > l && i < r) {
      // Use the previously calculated z value,
      // as long as that would keep us inside the z box:
      const auto prev = z[i - l];
      if (i + prev < r) {
        z[i] = prev;
        // std::cout << "(" << z[i] << ") ";
        continue;
      }
    }

    // See how long a prefix, if any,
    // starts at i:
    std::size_t a = 0;
    std::size_t j = i;
    for (; j < n; ++j, ++a) {
      if (s[j] != s[a]) {
        break;
      }
    }

    // if (a > 0) {
    l = i;
    r = j;
    //}

    z[i] = a;

    // std::cout << z[i] << " ";
  }
  // std::cout << std::endl;

  return z;
}

static std::vector<std::size_t>
find(const std::string& str, const std::string& pat) {
  std::vector<std::size_t> result;

  const auto s = pat + "$" + str;
  const auto z = make_z(s);

  // Find the first index whose z value is the length of the pattern:
  const auto n = s.size();
  const auto pat_size = pat.size();
  const auto start = pat_size + 1;
  for (std::size_t i = start; i < n; ++i) {
    if (z[i] == pat_size) {
      result.emplace_back(i - start);
    }
  }

  return result;
}

int
main() {
  const auto z = make_z("aabxaabxcaabxaabxay");
  const auto expected_z = std::vector<std::size_t>{
    0, 1, 0, 0, 4, 1, 0, 0, 0, 8, 1, 0, 0, 5, 1, 0, 0, 1, 0};
  assert(z == expected_z);

  assert(find("foo", "fo") == std::vector<std::size_t>({0}));
  assert(find("fop", "p") == std::vector<std::size_t>({2}));
  assert(find("xabcabzabc", "abc") == std::vector<std::size_t>({1, 7}));
  assert(find("foobarmoo", "oob") == std::vector<std::size_t>({1}));
  assert(find("foobarmoo", "bar") == std::vector<std::size_t>({3}));

  // Overlapping:
  assert(find("ababababa", "aba") == std::vector<std::size_t>({0, 2, 4, 6}));

  return EXIT_SUCCESS;
}
