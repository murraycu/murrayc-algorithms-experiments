// Build with:
// g++ -g -Wall -Werror -Wextra -Wpedantic -Wshadow --std=c++11 test.cc -o prog

#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>

typedef unsigned int type_value;

typedef std::vector<type_value> type_vec_coins;

template<typename T>
void print_vec(const std::vector<T>& vec)
{
  for(auto num : vec)
  {
    std::cout << num << ", ";
  }
}

int main()
{
  type_vec_coins coins{20, 50, 50, 20, 10, 100, 5, 2, 2, 1};

  //Sort largest first:
  std::sort(coins.begin(), coins.end(),
    [](const type_value& a, const type_value& b)
    {
      return a > b;
    });

  const type_value needed_value = 173;

  //Use a greedy algorithm, taking the biggest values first,
  //as long as they don't push us over the target value.
  //However, this only works for "canonical" coin systems:
  //http://cs.stackexchange.com/a/6625/40929
  //Otherwise it is a knapsack problem.
  type_value total = 0;
  type_vec_coins coins_used;
  for(auto value : coins)
  {
    if((total + value) > needed_value)
      continue;

    total += value;
    coins_used.emplace_back(value);

    if(total >= needed_value)
      break;
  }

  if(total > needed_value)
  {
    std::cerr << "Cannot make the needed value: " << needed_value << std::endl;

    std::cout << "Coins attempted: ";
    print_vec(coins_used);
    std::cout << std::endl;
  }
  else
  {
    std::cout << "Coins used for needed value: " << needed_value << ": ";
    print_vec(coins_used);
    std::cout << std::endl;
  }

  return EXIT_SUCCESS;
}


