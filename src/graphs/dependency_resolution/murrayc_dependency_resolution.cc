// Build with:
// g++ -g -Wall -Werror -Wextra -Wpedantic -Wshadow --std=c++11 test.cc -o prog

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/*
static
void indent(int level)
{
  std::cout << "level: " << level;
  for(int l = 0; l < level; ++l) {
    std::cout << "  ";
  }
}
*/

template <typename T>
void
print_vec(const std::vector<T>& vec) {
  for (auto num : vec) {
    std::cout << num << ", ";
  }
}

class DependencyResolution {
public:
  typedef std::string Id;
  typedef std::unordered_set<Id> type_ids; // package names

  // Map of package names to their dependencies:
  typedef std::unordered_map<Id, type_ids> type_packages;

  explicit DependencyResolution(const type_packages& packages)
  : packages_(packages), sequence_(0), finish_(false) {}

  std::vector<Id>
  get_build_sequence(const Id& package_to_build);

private:
  void
  dfs(const Id& id);

  void
  clear();

  typedef std::unordered_set<Id> type_children;
  type_children
  get_children(const Id& id);

  // These could be pure virtual methods in a generic DFS base class:
  void
  process_node_pre(const Id& id);
  void
  process_edge(const Id& parent_id, const Id& id);
  void
  process_node_post(const Id& id);

  bool
  is_discovered(const Id& id) const;
  bool
  is_processed(const Id& id) const;

  void
  set_discovered(const Id& id);
  void
  set_processed(const Id& id);

  const type_packages packages_;

  int sequence_;
  std::unordered_map<Id, int> discovered_;
  std::unordered_map<Id, int> processed_;
  bool finish_;
};

void
DependencyResolution::clear() {
  sequence_ = 0;
  discovered_.clear();
  processed_.clear();

  finish_ = false;
}

std::vector<DependencyResolution::Id>
DependencyResolution::get_build_sequence(const Id& package_to_build) {

  clear();

  dfs(package_to_build);

  /*
    for(const auto& iter : package_depths_)
    {
      std::cout << "package: " << iter.first << ", depth: " << iter.second <<
    std::endl;
    }
  */

  std::vector<Id> result;
  for (const auto& the_pair : processed_) {
    result.emplace_back(the_pair.first);
  }
  std::sort(result.begin(), result.end(), [this](const auto& a, const auto& b) {
    return processed_[a] < processed_[b];
  });

  return result;
}

DependencyResolution::type_children
DependencyResolution::get_children(const Id& id) {
  const auto iter = packages_.find(id);
  if (iter == packages_.end()) {
    std::cout << "Unlisted dependency: " << id << std::endl;
    finish_ = true;
    return type_children();
  }

  return iter->second;
}

void
DependencyResolution::process_edge(const Id& /* parent_id */, const Id& id) {
  // We have already discovered the node but not yet processed it.
  // This can only happen if we have looped back around behind ourself.
  if (is_discovered(id) && !is_processed(id)) {
    std::cout << "Circular dependency: " << id << std::endl;
    finish_ = true;
  }
}

bool
DependencyResolution::is_discovered(const Id& id) const {
  return discovered_.count(id);
}

bool
DependencyResolution::is_processed(const Id& id) const {
  return processed_.count(id);
}

void
DependencyResolution::set_discovered(const Id& id) {
  // std::cout << "set_discovered(): " << id << std::endl;

  // Store the entry time:
  discovered_.emplace(id, sequence_);
  ++sequence_;
}

void
DependencyResolution::set_processed(const Id& id) {
  // std::cout << "set_processed(): " << id << std::endl;

  // Store the exit time:
  processed_.emplace(id, sequence_);
  ++sequence_;
}

void
DependencyResolution::process_node_pre(const Id& /* id */) {}

void
DependencyResolution::process_node_post(const Id& /* id */) {
  // std::cout << "package: " << id << std::endl;
  // processed_.emplace(id);
}

void
DependencyResolution::dfs(const Id& id) {
  // std::cout << "dfs: " << id << std::endl;

  set_discovered(id);
  process_node_pre(id);

  for (const auto& dependency : get_children(id)) {
    // std::cout << "dependency: " << dependency << std::endl;
    if (!is_discovered(dependency)) {
      process_edge(id, dependency);
      dfs(dependency);
    } else if (!is_processed(dependency)) {
      process_edge(id, dependency);
    } else {
      // std::cout << "Already processed: " << dependency << std::endl;
    }

    // Finish early if necessary:
    if (finish_)
      break;
  }

  set_processed(id);
  process_node_post(id);
}

int
main() {
  const DependencyResolution::type_packages packages{
    {"gtkmm", {"glibmm", "pangomm", "atkmm", "libsigc++", "gtk+"}},
    {"libsigc++", {}}, {"glib", {}}, {"pango", {"glib"}}, {"atk", {"glib"}},
    // This circular dependency will be found: {"atk", {"glib", "gtk+"}},
    {"gtk+", {"glib", "pango", "atk"}},
    {"pangomm",
      {"glibmm", "libsigc++", "pango"}}, // Unnecessary libsigc++ dependency.
    {"glibmm", {"libsigc++", "glib"}},
    {"atkmm", {"glibmm", "atk"}},
    {"libxml++",
      {"glibmm", "libxml", "libsigc++"}} // Unnecessary libsigc++ dependency.
  };

  /*
    const DependencyResolution::type_packages packages{
      {"gtkmm", {"alibsigc++", "glibmm"}},
      {"glibmm", {"alibsigc++"}},
      {"alibsigc++", {}}
    };
  */

  // Use Depth-First-Search to get a topological sorting for the graph's nodes,
  // marking each node (package name) with an increasing sequence _after_ we
  // have
  // fully processed it - meaning after we have discovered and processed all
  // its children.

  // For instance, the start node will only be marked as processed
  // after all its child nodes have been marked as processed.
  // Therefore, the node (package name) with the lowest sequence number will
  // be the first one we should build.
  //
  // This works because, if we reach a node again via another path
  //(if another package depends on a package that we've already processed),
  // then that package will already have a lower sequence number from when it
  // was processed.
  DependencyResolution resolution(packages);

  const auto package_name = "gtkmm";
  const auto sequence = resolution.get_build_sequence(package_name);

  std::cout << "Build sequence for: " << package_name << ":" << std::endl;
  for (const auto& package : sequence) {
    std::cout << package << std::endl;
  }

  return EXIT_SUCCESS;
}
