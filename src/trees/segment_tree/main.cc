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
    T_Value min = T_Value();
    root = add_node_from_vector(root_lo, root_hi, values, min);
    assert(root->min == min);
  }

  /**
   * Find the minimum value in the tree, within the specifed range of keys.
   * In O(log(n)) time.
   *
   * @param start
   * @param end inclusive.
   */
  std::pair<bool, T_Value>
  min(std::size_t start, std::size_t end) const {
    if (!root) {
      return {false, T_Value()};
    }

    return min_from_node(root, root_lo, root_hi, start, end);
  }

  void remove(T_Key key) {
    if (!root) {
      return;
    }

    const auto summary = remove_and_get_min_from_node(root, root_lo, root_hi, key);
    if (summary.to_delete) {
      delete root;
      root = nullptr;
      return;
    }

    root->min = summary.min;
  }

private:
  /** Add the node for the middle values,
   * and its child nodes for the left and right parts.
   * Using the indices as the keys.
   */
  static Node*
  add_node_from_vector(std::size_t lo, std::size_t hi, const std::vector<T_Value>& values, T_Value& min) {
    const auto mid = lo + ((hi - lo) / 2);
    const auto& midval = values[mid];
    auto result = new Node(mid, midval);

    min = midval;

    if (lo != hi) {
      // The left range, including mid:
      T_Value min_left = T_Value();
      result->left = add_node_from_vector(lo, mid, values, min_left);
      min = std::min(min, min_left);

      // The right range, not including mid:
      if (mid < hi) {
        T_Value min_right = T_Value();
        result->right = add_node_from_vector(mid + 1, hi, values, min_right);
        min = std::min(min, min_right);
      }
    }

    // Store the range's minimum:
    // The actual range for this node is implict depending on
    // whether it is the left or right of the parent.
    result->min = min;

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

  class NodeSummary {
  public:
    bool contributes = false;
    T_Value min = T_Value();
    bool to_delete = false;
  };

  /**
   * @param start
   * @param end inclusive.
   */
  static std::pair<bool, T_Value>
  min_from_node(Node* node, std::size_t node_lo, std::size_t node_hi, std::size_t start, std::size_t end) {
    if (!node ||
      node_lo > node_hi ||
      node_hi < node_lo) {
      return {false, T_Value()};
    }

    // Total overlap of start/end over node's lo/hi:
    if (start <= node_lo && end >= node_hi) {
      return {true, node->min};
    }

    // No overlap:
    if (node_lo > end  || node_hi < start) {
      return {false, T_Value()};
    }

    // Partial overlap:
    // Look at left and right:
    const auto mid = node_lo + ((node_hi - node_lo) / 2);
    const auto l = min_from_node(node->left, node_lo, mid, start, end);
    const auto r = min_from_node(node->right, mid + 1, node_hi, start, end);
    if (l.first && r.first) {
      return {true, std::min(l.second, r.second)};
    } else if (l.first) {
      return l;
    } else {
      return r;
    }
  }

  static NodeSummary 
  remove_and_get_min_from_node(Node* node, T_Key node_lo, T_Key node_hi, T_Key key) {
    if (!node) {
      return {false, T_Value(), false};
    }

    // Total overlap:
    // Remove this key by removing this node:
    if (node_lo == key && node_hi == key) {
      return {false, T_Key(), true /* delete it */};
    }

    // No overlap:
    if (node_lo > key  || node_hi < key) {
      return {true, node->min, false};
    }

    const auto mid = node_lo + ((node_hi - node_lo) / 2);
    const auto l = remove_and_get_min_from_node(node->left, node_lo, mid, key);
    if (l.to_delete) {
      delete node->left;
      node->left = nullptr;
    } else if (l.contributes) {
      node->left->min = l.min;
    }

    const auto r = remove_and_get_min_from_node(node->right, mid + 1, node_hi, key);
    if (r.to_delete) {
      delete node->right;
      node->right = nullptr;
    } else if (r.contributes) {
      node->right->min = r.min;
    }

    if (l.contributes && r.contributes) {
      return {true, std::min(l.min, r.min), false};
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
  assert(st.min(2, 4).second == 2);

  st.remove(4);
  assert(st.min(2, 4).second == 4);
}

int main() {
  test_min();
  test_remove_and_min();

  return EXIT_SUCCESS;
}

