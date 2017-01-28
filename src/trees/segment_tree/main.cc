#include <type_traits>
#include <vector>
#include <cstdlib>
#include <cassert>

// Note: This can be implemented more efficiently
// with an array instead of tree nodes,
// but that might make implementation of add() and
// remove() inefficient.

template <typename T_Key, typename T_Value>
class SegmentTree {
private :
  class NodeSummary {
  public:
    bool contributes = false;

    // The minimum value in this node's range:
    T_Value min = T_Value();

    // The maximum value in this node's range:
    T_Value max = T_Value();

    // The number of keys in this node's range:
    std::size_t count = 0;
  };

  class Node {
  public:
    Node(T_Key in_key, T_Value in_value)
    : key(in_key),
      value(in_value) {
        summary.contributes = true;
    }

    T_Key key = T_Key();
    T_Value value = T_Value();

    // TODO: Node doesn't use the contributes member,
    // so split those out when C++17 makes it more convenient
    // to return a std::tuple<contributes, summary>.
    NodeSummary summary;

    // Smaller sub-ranges are in the children:
    Node* left = nullptr;
    Node* right = nullptr;
  };

public:
  SegmentTree() {
  }

  ~SegmentTree() {
    delete_node(root);
  }

  /** Build a Segment Tree with array indices as keys,
   * with the specified associated values.
   *
   * In O(log(n)) time.
   */
  SegmentTree(const std::vector<T_Value>& values) {
    /* TODO: Allow other suitable types?
    static_assert(std::is_same<T_Key, std::size_t>::value,
      "This constructor should only be used if T_Key is std::size_t, "
      "because it uses the vector indices as keys");
    */

    if (values.empty()) {
      return;
    }

    root_lo = 0;
    root_hi = values.size() - 1;
    NodeSummary summary;
    root = add_node_from_vector(root_lo, root_hi, values, summary);
    assert(root->summary.min == summary.min);
    assert(root->summary.max == summary.max);
    assert(root->summary.count == summary.count);
  }

  /**
   * Find the minimum value in the tree, within the specifed range of keys.
   * In O(log(n)) time.
   *
   * @param start
   * @param end inclusive.
   * @result {some key exists, minimum}
   */
  std::pair<bool, T_Value>
  min(T_Key start, T_Key end) const {
    if (!root) {
      return {false, T_Value()};
    }

    const auto summary = summary_from_node(root, root_lo, root_hi, start, end);
    return {summary.contributes, summary.min};
  }

  /**
   * Find the maximum value in the tree, within the specifed range of keys.
   * In O(log(n)) time.
   *
   * @param start
   * @param end inclusive.
   * @result {some key exists, maximum}
   */
  std::pair<bool, T_Value>
  max(T_Key start, T_Key end) const {
    if (!root) {
      return {false, T_Value()};
    }

    const auto summary = summary_from_node(root, root_lo, root_hi, start, end);
    return {summary.contributes, summary.max};
  }

  /** Count the items whose keys are in the specified range.
   *
   * @param start
   * @param end inclusive.
   */
  std::size_t
  count(T_Key start, T_Key end) const {
    if (!root) {
      return 0;
    }

    const auto summary = summary_from_node(root, root_lo, root_hi, start, end);
    return summary.count;
  }

  /** Count the missing items in the specified range.
   *
   * @param start
   * @param end inclusive.
   */
  std::size_t
  count_empty(T_Key start, T_Key end) const {
    const auto full = end - start + 1;
    if (!root) {
      // They are al missing:
      return full;
    }

    if (end < root_lo || start > root_hi) {
      // They are al missing:
      return full;
    }

    const auto c = count(start, end);
    return full - c;
  }

  /**
   * Find the kth missing key in the tree.
   * In O(log(n)) time.
   *
   * @param start
   * @param end inclusive.
   * @result {a kth key is missing, key}
   */
  std::pair<bool, T_Key>
  get_kth_empty(std::size_t k) const {
    if (!root) {
      // Nothing is missing from nothing.
      return {false, T_Key()};
    }

    auto lo = root_lo;
    auto hi = root_hi;

    // Gradually move into the range where the missing value would have to be,
    // by examining the count and the expected count in the halves, until we
    // are looking at a single-item range, and looking for just one missing item (k=0).
    //
    // For instance (with indices as keys, and values conveniently equal to those keys):
    //
    // If k = 0th with (0, 1, 2, 3, 4, 5):
    //   lo/mid/hi=0,2,5 then 3,4,5, then ends on 5,5, when k=0, but count(5) = 1.
    //
    // If k = 0th with (0, _, 2, 3, 4):
    //   lo/mid/hi=0,2,5 then 0,1,2, then 0,0,1, then ends on 1,1, when k=0 and count(1) = 0.
    //
    // If k = 1th with (0, _, 2, 3, _, 5):
    //   lo/mid/hi=0,2,5 then 3,4,5, then 3,3,4, then ends on 4,4, when k=0 and count(4) = 0.
    while (lo < hi) {
      const auto mid = lo + ((hi - lo) / 2);
      const auto lc = count(lo, mid);

      const auto halfn = mid - lo + 1;
      const auto missing = halfn - lc;

      if (missing >= (k + 1)) {
        hi = mid;
      } else {
        k -= missing;
        lo = mid + 1;
      }
    }

    // Now lo == hi.
    // And either that key is missing or it is not.
    const auto c = count(lo, lo);
    if (k == 0 &&
        c == 0) {
      return {true, lo};
    }

    return {false, T_Key()};
  }

  void remove(T_Key key) {
    if (!root) {
      return;
    }

    const auto del = remove_and_update_summary_from_node(root, root_lo, root_hi, key);
    if (del) {
      delete root;
      root = nullptr;
      return;
    }
  }

private:
  /** Add the node for the middle values,
   * and its child nodes for the left and right parts.
   * Using the indices as the keys.
   */
  static Node*
  add_node_from_vector(std::size_t lo, std::size_t hi, const std::vector<T_Value>& values, NodeSummary& summary) {
    const auto mid = lo + ((hi - lo) / 2);
    const auto& midval = values[mid];
    auto result = new Node(mid, midval);

    summary.contributes = true;
    summary.min = midval;
    summary.max = midval;
    summary.count = 1;

    if (lo != hi) {
      // The left range, including mid:
      result->left = add_node_from_vector(lo, mid, values, summary);

      // The right range, not including mid:
      if (mid < hi) {
        NodeSummary summary_right;
        result->right = add_node_from_vector(mid + 1, hi, values, summary_right);
        summary = summary_of_children(summary, summary_right);
      }
    }

    // Store the range's minimum, count, etc:
    // The actual range for this node is implict depending on
    // whether it is the left or right of the parent.
    result->summary = summary;

    return result;
  }

  static void
  delete_node(Node* node) {
    if (!node) {
      return;
    }

    delete_node(node->left);
    delete_node(node->right);
    delete node;
  }

  /**
   * @param start
   * @param end inclusive.
   */
  static NodeSummary
  summary_from_node(Node* node, T_Key node_lo, T_Key node_hi, T_Key start, T_Key end) {
    if (!node ||
      node_lo > node_hi ||
      node_hi < node_lo) {
      return {false};
    }

    // Total overlap of start/end over node's lo/hi:
    if (start <= node_lo && end >= node_hi) {
      return node->summary;
    }

    // No overlap:
    if (node_lo > end  || node_hi < start) {
      return {false};
    }

    // Partial overlap:
    // Look at left and right:
    const auto mid = node_lo + ((node_hi - node_lo) / 2);
    const auto l = summary_from_node(node->left, node_lo, mid, start, end);
    const auto r = summary_from_node(node->right, mid + 1, node_hi, start, end);
    return summary_of_children(l, r);
  }

  /**
   * @result True if the caller should delete and forget about this child node.
   */
  static bool
  remove_and_update_summary_from_node(Node* node, T_Key node_lo, T_Key node_hi, T_Key key) {
    if (!node) {
      return false;
    }

    // Total overlap:
    // Remove this key by removing this node:
    if (node_lo == key && node_hi == key) {
      return true; /* delete it */
    }

    // No overlap:
    if (node_lo > key  || node_hi < key) {
      return false;
    }

    const auto mid = node_lo + ((node_hi - node_lo) / 2);
    const auto ldelete = remove_and_update_summary_from_node(node->left, node_lo, mid, key);
    if (ldelete) {
      delete node->left;
      node->left = nullptr;
    }

    const auto rdelete = remove_and_update_summary_from_node(node->right, mid + 1, node_hi, key);
    if (rdelete) {
      delete node->right;
      node->right = nullptr;
    }

    node->summary = summary_of_children(node->left, node->right);

    // Delete this node if its children are both empty:
    const auto count = (node->left ? node->left->summary.count : 0) +
      (node->right ? node->right->summary.count : 0);
    if (count == 0) {
      return true;
    }

    return false;
  }

  static NodeSummary
  summary_of_children(const NodeSummary& lsum, const NodeSummary& rsum) {
    NodeSummary summary;
    if (lsum.contributes && rsum.contributes) {
      const auto min = std::min(lsum.min, rsum.min);
      const auto max = std::max(lsum.max, rsum.max);
      const auto count = lsum.count + rsum.count;
      summary = {true, min, max, count};
    } else if (lsum.contributes) {
      summary = lsum;
    } else {
      summary = rsum;
    }

    return summary;
  }

  static NodeSummary
  summary_of_children(const Node* l, const Node* r) {
    if (l && r) {
      return summary_of_children(l->summary, r->summary);
    }

    if (l) {
      return l->summary;
    } else if (r) {
      return r->summary;
    } else {
      return {false};
    }
  }


  Node* root = nullptr;
  T_Key root_lo = T_Key();
  T_Key root_hi = T_Key();
};

static void
test_min() {
  // RMQ (Range Minium Query)
  // to find the lowest value in the range:
  std::vector<int> values = {-1, 3, 4, 0, 2, 1};

  // A segment tree mapping indices (keys) to values,
  // which can return the minimum value in a range of indices.
  SegmentTree<std::size_t, int> st(values);

  /* TODO: Implement this alternative:
  for (auto i = 0u; i < values.size(); ++i) {
    st.add(values[i], i);
  }
  */

  assert(st.min(10, 11).first == false);

  assert(st.min(1, 2).second == 3);

  assert(st.min(2, 4).first == true);
  assert(st.min(2, 4).second == 0);
  assert(st.min(0, 4).first == true);
  assert(st.min(0, 4).second == -1);

  assert(st.min(0, 5).first == true);
  assert(st.min(0, 5).second == -1);

  assert(st.min(7, 8).first == false);

  assert(st.min(0, 2).second == -1);
  assert(st.min(0, 1).second == -1);
  assert(st.min(1, 2).second == 3);
  assert(st.min(4, 5).second == 1);
  assert(st.min(3, 5).second == 0);
}

static void
test_remove_and_min() {
  // RMQ (Range Minium Query)
  // to find the lowest value in the range:
  std::vector<int> values = {-1, 3, 4, 0, 2, 1};

  // A segment tree mapping indices (keys) to values,
  // which can return the minimum value in a range of indices.
  SegmentTree<std::size_t, int> st(values);

  assert(st.min(2, 4).first == true);
  assert(st.min(2, 4).second == 0);

  // Remove the 0 at index 3.
  // This doesn't change the index of entries to the right
  // - the SegmentTree knows about these simply as key values.
  st.remove(3);
  assert(st.min(3, 3).first == false);
  assert(st.min(2, 4).second == 2);

  st.remove(4);
  assert(st.min(4, 4).first == false);
  assert(st.min(2, 4).second == 4);
}

static void
test_max() {
  std::vector<int> values = {-1, 3, 4, 0, 2, 1};

  // A segment tree mapping indices (keys) to values,
  // which can return the maximum value in a range of indices.
  SegmentTree<std::size_t, int> st(values);

  assert(st.max(10, 11).first == false);

  assert(st.max(1, 2).first == true);
  assert(st.max(1, 2).second == 4);

  assert(st.max(2, 4).first == true);
  assert(st.max(2, 4).second == 4);
  assert(st.max(0, 4).first == true);
  assert(st.max(0, 4).second == 4);

  assert(st.max(0, 5).first == true);
  assert(st.max(0, 5).second == 4);

  assert(st.max(7, 8).first == false);

  assert(st.max(0, 2).second == 4);
  assert(st.max(0, 1).second == 3);
  assert(st.max(1, 2).second == 4);
  assert(st.max(4, 5).second == 2);
  assert(st.max(3, 5).second == 2);
}

static void
test_remove_and_max() {
  // RMQ (Range Minium Query)
  // to find the lowest value in the range:
  std::vector<int> values = {-1, 3, 4, 0, 2, 1};

  // A segment tree mapping indices (keys) to values,
  // which can return the minimum value in a range of indices.
  SegmentTree<std::size_t, int> st(values);

  assert(st.max(2, 4).first == true);
  assert(st.max(2, 4).second == 4);

  // Remove the 4 at index 2.
  // This doesn't change the index of entries to the right
  // - the SegmentTree knows about these simply as key values.
  st.remove(2);
  assert(st.max(2, 2).first == false);
  assert(st.max(2, 4).first == true);
  assert(st.max(2, 4).second == 2);

  st.remove(4);
  assert(st.max(4, 4).first == false);
  assert(st.max(2, 4).second == 0);
}

static void
test_count() {
  std::vector<int> values = {-1, 3, 4, 0, 2, 1};

  SegmentTree<std::size_t, int> st(values);

  assert(st.count(10, 11) == 0);

  assert(st.count(0, 5) == 6);
  assert(st.count_empty(0, 5) == 0);
  assert(st.count(1, 2) == 2);

  st.remove(2);
  assert(st.count(2, 2) == 0);
  assert(st.count(1, 2) == 1);
  assert(st.count_empty(1, 5) == 1);
}

static void
test_kth_empty() {
  std::vector<int> values = {-1, 3, 4, 0, 2, 1};

  SegmentTree<std::size_t, int> st(values);

  assert(st.get_kth_empty(0).first == false);
  st.remove(1);
  assert(st.get_kth_empty(0).first == true);
  assert(st.get_kth_empty(0).second == 1);

  st.remove(4);
  assert(st.get_kth_empty(0).first == true);
  assert(st.get_kth_empty(0).second == 1);
  assert(st.get_kth_empty(1).first == true);
  assert(st.get_kth_empty(1).second == 4);
}

int main() {
  test_min();
  test_remove_and_min();

  test_max();
  test_remove_and_max();

  test_count();
  test_kth_empty();

  return EXIT_SUCCESS;
}

