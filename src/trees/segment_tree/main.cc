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
  class Node {
  public:
    Node(T_Key in_key, T_Value in_value)
    : key(in_key),
      value(in_value) {
    }

    T_Key key = T_Key();
    T_Value value = T_Value();

    // The minimum value in this node's range:
    T_Value min = T_Value();

    // The number of keys in this node's range:
    std::size_t count = 0;

    // Smaller sub-ranges are in the children:
    Node* left = nullptr;
    Node* right = nullptr;
  };

  class NodeSummary {
  public:
    bool contributes = false;
    T_Value min = T_Value();
    std::size_t count = 0;
    bool to_delete = false;
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
    assert(root->min == summary.min);
    assert(root->count == summary.count);
  }

  /**
   * Find the minimum value in the tree, within the specifed range of keys.
   * In O(log(n)) time.
   *
   * @param start
   * @param end inclusive.
   */
  std::pair<bool, T_Value>
  min(T_Key start, T_Key end) const {
    if (!root) {
      return {false, T_Value()};
    }

    const auto summary = summary_from_node(root, root_lo, root_hi, start, end);
    return {summary.contributes, summary.min};
  }


  std::size_t count(T_Key start, T_Key end) const {
    if (!root) {
      return 0;
    }

    const auto summary = summary_from_node(root, root_lo, root_hi, start, end);
    return summary.count;
  }

  void remove(T_Key key) {
    if (!root) {
      return;
    }

    const auto summary = remove_and_get_summary_from_node(root, root_lo, root_hi, key);
    if (summary.to_delete) {
      delete root;
      root = nullptr;
      return;
    }

    use_summary(root, summary);
  }

private:
  static void
  use_summary(Node* node, const NodeSummary& summary) {
    node->min = summary.min;
    node->count = summary.count;
  }

  /** Add the node for the middle values,
   * and its child nodes for the left and right parts.
   * Using the indices as the keys.
   */
  static Node*
  add_node_from_vector(std::size_t lo, std::size_t hi, const std::vector<T_Value>& values, NodeSummary& summary) {
    const auto mid = lo + ((hi - lo) / 2);
    const auto& midval = values[mid];
    auto result = new Node(mid, midval);

    summary.min = midval;
    summary.count = 1;

    if (lo != hi) {
      // The left range, including mid:
      NodeSummary summary_left;
      result->left = add_node_from_vector(lo, mid, values, summary_left);
      summary.min = std::min(summary.min, summary_left.min);
      summary.count = summary_left.count;

      // The right range, not including mid:
      if (mid < hi) {
        NodeSummary summary_right;
        result->right = add_node_from_vector(mid + 1, hi, values, summary_right);
        summary.min = std::min(summary.min, summary_right.min);
        summary.count += summary_right.count;
      }
    }

    // Store the range's minimum, count, etc:
    // The actual range for this node is implict depending on
    // whether it is the left or right of the parent.
    use_summary(result, summary);

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
      return {true, node->min, node->count};
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
    if (l.contributes && r.contributes) {
      return {true, std::min(l.min, r.min), l.count + r.count};
    } else if (l.contributes) {
      return l;
    } else {
      return r;
    }
  }

  static NodeSummary
  remove_and_get_summary_from_node(Node* node, T_Key node_lo, T_Key node_hi, T_Key key) {
    if (!node) {
      return {false};
    }

    // Total overlap:
    // Remove this key by removing this node:
    if (node_lo == key && node_hi == key) {
      return {false, T_Key(), 0, true /* delete it */};
    }

    // No overlap:
    if (node_lo > key  || node_hi < key) {
      return {true, node->min, node->count, false};
    }

    const auto mid = node_lo + ((node_hi - node_lo) / 2);
    const auto l = remove_and_get_summary_from_node(node->left, node_lo, mid, key);
    if (l.to_delete) {
      delete node->left;
      node->left = nullptr;
    } else if (l.contributes) {
      use_summary(node->left, l);
    }

    const auto r = remove_and_get_summary_from_node(node->right, mid + 1, node_hi, key);
    if (r.to_delete) {
      delete node->right;
      node->right = nullptr;
    } else if (r.contributes) {
      use_summary(node->right, r);
    }

    if (l.contributes && r.contributes) {
      return {true, std::min(l.min, r.min), l.count + r.count, false};
    } else if (l.contributes) {
      return l;
    } else {
      return r;
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
test_count() {
  std::vector<int> values = {-1, 3, 4, 0, 2, 1};

  SegmentTree<std::size_t, int> st(values);

  assert(st.count(10, 11) == 0);

  assert(st.count(0, 5) == 6);
  assert(st.count(1, 2) == 2);

  st.remove(2);
  assert(st.count(2, 2) == 0);
  assert(st.count(1, 2) == 1);
}

int main() {
  test_min();
  test_remove_and_min();

  test_count();

  return EXIT_SUCCESS;
}

