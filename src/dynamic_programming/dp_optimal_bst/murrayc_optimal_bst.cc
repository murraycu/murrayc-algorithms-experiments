#include <limits>
#include <vector>
#include <cassert>
#include <cstdlib>

class ItemAndFrequency {
public:
  // The name of the item.
  char item;

  // The percentage of searches that are for this item:
  int percentage; // 0 to 100.
};

using type_items = std::vector<ItemAndFrequency>;

class SubSolution {
public:
  int value;
  std::size_t root;
};

// Find the BST that will have the overall minimum cost for all searches,
// given the frequencies of searches for the various items.
//
// A search has a higher cost if it must traverse more nodes (if the item is at a deeper level).
// The cost of the BST is therefore the sum of (node's frequency * node's depth).
// This algorithm doesn't multiply directly - instead it adds the frequency once for each level that the node appears in.
static SubSolution
calc_optimal_bst(const type_items& items) {
  const auto n = items.size();

  // sub[s items, starting at i]:
  std::vector<std::vector<SubSolution>> sub(n + 1, std::vector<SubSolution>(n));

  // s = 1:
  {
    auto& subs = sub[1];
    for (auto i = 0ul; i < n; ++i) {
      subs[i] = {items[i].percentage, i};
    }
  }

  // Try every range of length s:
  for (auto s = 2ul; s <= n; ++s) {
    auto& subs = sub[s];

    // Try every range (of length s) starting at position i:
    for (auto i = 0ul; i < n; ++i) {
      auto& subsi = subs[i];

      // TODO: This should reduce the overall time to O(n^2):
      // https://stackoverflow.com/questions/16987670/dynamic-programming-why-knuths-improvement-to-optimal-binary-search-tree-on2
      // but that needs us to index by the start and end, not by the start and length.

      // Try every root in this range:
      const auto end = i + s;
      subsi.value = std::numeric_limits<int>::max();

      for (auto j = i; j < end; ++j) {
        const auto sl = j - i;
        const auto sr = s - sl - 1;

        // Sum the left and right sides of the tree.
        // (This uses the sub[size = 0] zero-cost base cases. We could instead just check for s == 0).
        // We don't add the root's cost here, because that will be part of the sum of all frequences (sumfreq).
        const auto cost = sub[sl][i].value + sub[sr][j + 1].value;
        if (cost < subsi.value) {
          subsi = {cost,  j};
        }
      }

      // Add the sum of all frequences in this range.
      // This adds the frequency once for each level that the item appears in.
      int sumfreq = 0;
      for (auto j = i; j < end; ++j) {
        sumfreq += items[j].percentage;
      }
      subsi.value += sumfreq;
    }
  }

  return sub[n][0];
}

int main() {

  {
    // Note that these must be sorted,
    // so we can choose a root r and know that the items
    // before it will be in its left sub-tree, and the items
    // after it will be in the right sub-tree.
    const type_items items = {
      {'a', 34}, {'b', 50}};
    const auto result = calc_optimal_bst(items);
    assert(result.root == 1);
    assert(result.value == 118);
  }

  {
    // Note that these must be sorted,
    // so we can choose a root r and know that the items
    // before it will be in its left sub-tree, and the items
    // after it will be in the right sub-tree.
    const type_items items = {
      {'a', 34}, {'b', 8}, {'c', 50}};
    const auto result = calc_optimal_bst(items);
    assert(result.root == 2);
    assert(result.value == 142);
  }

  {
    // Note that these must be sorted,
    // so we can choose a root r and know that the items
    // before it will be in its left sub-tree, and the items
    // after it will be in the right sub-tree.
    const type_items items = {
      {'a', 11}, {'b', 10}, {'c', 12}, {'d', 22}, {'e', 18}};
    const auto result = calc_optimal_bst(items);
    assert(result.root == 3);
    assert(result.value == 147);
  }

  return EXIT_SUCCESS;
}

