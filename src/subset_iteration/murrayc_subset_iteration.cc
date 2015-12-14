#include <iostream>
#include <vector>
#include <cstdlib>
#include "subset_iterator.h"

template<typename T>
void print_vec(const std::vector<T>& vec)
{
  for(auto num : vec)
  {
    std::cout << num << ", ";
  }
}

int main() {

  using type_vec = std::vector<int>;
  const type_vec set = {1, 2, 3, 4, 5};
  std::cout << "Whole set: ";
  print_vec(set);
  std::cout << std::endl;

  for(auto iter = SubsetIterator<type_vec>::create(set.size(), 3);
    iter != SubsetIterator<type_vec>::end(); ++iter) {
    const auto subset = iter.get_subset(set);
    std::cout << "Subset: ";
    print_vec(subset);
    std::cout << std::endl;
  }

  return EXIT_SUCCESS;
}
