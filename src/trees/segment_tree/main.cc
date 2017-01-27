#include <vector>
#include <cstdlib>
#include <cassert>

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
    T_Key lo = T_Key();
    T_Key hi = T_Key();
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
    if (values.empty()) {
      return;
    }

    std::size_t lo = 0;
    std::size_t hi = values.size() - 1;
    T_Value min = T_Value();
    root = add_node_from_vector(lo, hi, values, min);
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

    return min_from_node(root, start, end);
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

    // Store the range and its minimum:
    result->lo = lo;
    result->hi = hi;
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

  /**
   * @param start
   * @param end inclusive.
   */
  static std::pair<bool, T_Value>
  min_from_node(Node* node, std::size_t start, std::size_t end) {
    if (!node) {
      return {false, T_Value()};
    }

    // Total overlap of start/end over node's lo/hi:
    if (start <= node->lo && end >= node->hi) {
      return {true, node->min};
    }

    // No overlap:
    if (node->lo > end  || node->hi < start) {
      return {false, T_Value()};
    }

    // Partial overlap:
    // Look at left and right:
    const auto l = min_from_node(node->left, start, end);
    const auto r = min_from_node(node->right, start, end);
    if (l.first && r.first) {
      return {true, std::min(l.second, r.second)};
    } else if (l.first) {
      return l;
    } else {
      return r;
    }
  }

  Node* root = nullptr;
};

int main() {

  // RMQ (Range Minium Query)
  // to find the lowest value in the range:
  {
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

  return EXIT_SUCCESS;
}

