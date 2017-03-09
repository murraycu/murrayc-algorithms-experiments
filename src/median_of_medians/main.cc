#include <algorithm>
#include <cassert>
#include <vector>

constexpr std::size_t FIVE = 5;

/** Returns the index of the median of the 5 elements
 * starting at @a i.
 * This takes constant (O(1)) time, because 5 is our constant.
 */
static std::size_t
position_of_median_of_five(
  std::vector<int>& values, std::size_t start, std::size_t end) {
  assert(end - start <= FIVE);

  std::sort(std::begin(values) + start, std::begin(values) + end);
  return (end - start) / 2;
}

/**
 * Find the the value that has rank k,
 * modifying the container so that it is partitioned around that value.
 * So the value will end up at position k in @a values.
 *
 * This takes O(n) time.
 *
 * @param start The start of the range to examine and partition.
 * @param end One past the end of the range to examine and partition.
 * @param k The rank of the item to return.
 */
static int
select(
  std::vector<int>& values, std::size_t start, std::size_t end, std::size_t k) {
  // Base case:
  if (end - start <= FIVE) {
    return values[k];
  }

  auto j = start;

  // Iterate over all items, in groups of 5:
  for (auto i = start; i < end; i += FIVE) {
    // Get the median of this group of 5 values:
    const auto group_end = (i + FIVE) >= end ? end : i + FIVE;
    const auto imedian = position_of_median_of_five(values, i, group_end);

    // Move that median to the start of the whole range,
    // so they are all grouped together:
    std::swap(values[j], values[imedian]);
    ++j;
  }

  // Recurse on the median of medians,
  // selecting the middle value:
  // until the group at the start is itself 5 or less,
  // so we really have the median of medians:
  const auto subk = start + ((j - start) / 2);
  const auto median_of_medians = select(values, start, j, subk);

  // Use the median of medians to partition the whole range,
  // possibly finding that it is also the kth item of the whole range:
  // (The median of medians is not necessarily the actual median of the whole
  // range)
  // There is a high enough probability (but not as much as 50%) that this is in
  // the
  // first half to make the recursion result in an overall linear (O(n)) time.
  auto iter_pivot = std::partition(std::begin(values), std::end(values),
    [median_of_medians](int value) { return value < median_of_medians; });
  const std::size_t ipivot =
    std::distance(std::begin(values) + start, iter_pivot);

  // Recurse on one half:
  if (ipivot == k) {
    return values[ipivot];
  } else if (k > subk) {
    // The kth value is not in the set of values moved to the start:
    return select(values, subk + 1, end, k);
  } else {
    // The kth value is in the set of values moved to the start:
    return select(values, start, subk, k);
  }
}

/** Get the median of the values, getting the first median value if n is even.
 * This does not take an average of two possible medians if n is even.
 * This takes O(n) time.
 */
static int
median(std::vector<int>& values) {
  const auto n = values.size();
  if (n == 0) {
    return 0;
  }

  // For instance, for 0, 1, 2, we want 1.
  // and for 0, 1, we want 0.
  // We could instead choose to get two medians for even n,
  // and take a mean.
  const auto k = (n - 1) / 2;

  return select(values, 0, n, k);
}

int
main() {
  {
    std::vector<int> test = {1};
    assert(median(test) == 1);
  }

  {
    std::vector<int> test = {1, 2};
    assert(median(test) == 1);
  }

  {
    std::vector<int> test = {1, 2, 3};
    assert(median(test) == 2);
  }

  {
    std::vector<int> test = {1, 2, 3, 4, 5};
    assert(median(test) == 3);
  }

  {
    std::vector<int> test = {5, 4, 3, 2, 1};
    assert(median(test) == 3);
  }

  {
    std::vector<int> test = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    assert(median(test) == 5);
  }

  {
    std::vector<int> test = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    assert(median(test) == 5);
  }

  {
    std::vector<int> test = {63, 23, 56, 49, 48, 64, 16, 10, 13, 59, 55, 46, 98,
      69, 15, 31, 46, 15, 82, 57, 91, 5, 22, 100, 47, 3, 86, 10, 61, 54, 48, 30,
      2, 23, 29, 42, 4, 81, 43, 63, 16, 6, 59, 46, 52, 65, 7, 93, 3, 20, 79, 59,
      4, 64, 9, 59, 59, 44, 49, 65, 82, 34, 25, 17, 87, 32, 13, 21, 97, 30, 69,
      100, 14, 53, 61, 85, 98, 6, 15, 53, 100, 6, 19, 37, 36, 80, 25, 26, 57,
      29, 72, 34, 3, 30, 26, 39, 25, 40, 76, 25};
    assert(median(test) == 37);
  }

  return 0;
}
