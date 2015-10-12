// Build with:
// g++ -g -Wall -Werror -Wextra -Wpedantic -Wshadow --std=c++11 test.cc -o prog

#include <cstdlib>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>

typedef unsigned int type_value;

typedef std::vector<type_value> type_vec_coins;

static
void indent(int level)
{
  std::cout << "level: " << level;
  for(int l = 0; l < level; ++l) {
    std::cout << "  ";
  }
}

template<typename T>
void print_vec(const std::vector<T>& vec)
{
  for(auto num : vec)
  {
    std::cout << num << ", ";
  }
}

typedef type_vec_coins::size_type type_size;

class SubSolution
{
public:
  SubSolution()
  : value(0)
  {
  }

  explicit SubSolution(type_value value_in)
  : value(value_in)
  {}

  SubSolution(const SubSolution& src) = default;
  SubSolution& operator=(const SubSolution& src) = default;

  SubSolution(SubSolution&& src) = default;
  SubSolution& operator=(SubSolution&& src) = default;

  type_value value;
  type_vec_coins solution;
};


typedef std::vector<std::unordered_map<type_size, SubSolution>> type_sub_problems;

const type_value VALUE_INFINITY = std::numeric_limits<type_value>::max();

const type_value VALUE_ONE = 1; //1 coin has a value of 1.

SubSolution
calc_optimal_sub_problem(const type_vec_coins& items, type_size item_number,
  type_size items_count, type_value needed_value, type_sub_problems& sub_problems,
  int level)
{
  ++level;

  indent(level);
  std::cout << "calc_optimal_sub_problem(): item_number=" << item_number <<
    ", items_count=" << items_count << ", needed_value=" << needed_value << std::endl;

  //0 value with 0 coins.
  if(items_count == 0)
  {
    return SubSolution();
  }
  else if(items_count == 0)
  {
    return SubSolution();
  }
  else if(item_number == 0)
  {
    const auto value = items[item_number];
    SubSolution result;
    result.value = value;
    result.solution.emplace_back(value);
    return result;
  }

  //Return a cached (memoized) value if one exists:
  auto& map = sub_problems[item_number];
  const auto iter = map.find(items_count);
  if(iter != map.end())
  {
    return iter->second;
  }

  SubSolution result;

  //Otherwise calculate it:
  const auto case_dont_use_this_item =
    calc_optimal_sub_problem(items,
      item_number - 1, items_count,
      needed_value,
      sub_problems,
      level);
  if(case_dont_use_this_item.value >= needed_value) 
  {
    result = case_dont_use_this_item;

    //Cache it:
    map[items_count] = result;

    indent(level);
    std::cout << "matching result.value=" << result.value << std::endl;

    return result;
  }

  const auto this_item_value = items[item_number];
  indent(level);
  std::cout << "this_item_value: " << this_item_value << std::endl;

  auto case_use_this_item =
    calc_optimal_sub_problem(items,
      item_number - 1, items_count - VALUE_ONE,
      needed_value,
      sub_problems,
      level);
  case_use_this_item.value += this_item_value;

  //Ignore values over the exact needed value,
  //because we are aiming for an exact value, not a maximum:
  //if(case_use_this_item > needed_value)
  //{
  //  case_use_this_item = 0;
  //}

  if(case_use_this_item.value > case_dont_use_this_item.value)
  {
    result = case_use_this_item;
    result.solution.emplace_back(this_item_value);
  }
  else
  {
    result = case_dont_use_this_item;
  }

  //Cache it:
  map[items_count] = result;

  indent(level);
  std::cout << "result.value=" << result.value << std::endl;

  return result;
}

int main()
{
  type_vec_coins coins{50, 52, 10, 100, 5, 2, 2, 4, 6, 7, 15, 1};
  //type_vec_coins coins{100, 119};

  //Sort largest first:
  /*
  std::sort(coins.begin(), coins.end(),
    [](const type_value& a, const type_value& b)
    {
      return a > b;
    });
  */

  const type_value needed_value = 119;

  //Use a dynamic programming (optimal sub structure) algorithm,
  //because a greedy algorithm would only work with a "canonical" coin system:
  //http://cs.stackexchange.com/a/6625/40929
  //This is like the knapsack problem, but where:
  //- coin value is like knapsack item value.
  //- coin count is like knapsack capacity.
  //Unlike with the knapsack problem, we want the minimum coin count for the needed value,
  //instead of the maximum value for the available (or less) weigtht.
  //
  //Use a recursive memoization technique, instead of filling a count*needed_value
  //array, because we shouldn't need to calculate all possibilities.
  const auto items_count = coins.size();
  type_sub_problems sub_problems(items_count + 1);
  const auto solution = calc_optimal_sub_problem(coins, items_count - 1, items_count, needed_value,
    sub_problems, 0);
  std::cout << "value_reached: " << solution.value << std::endl <<
    "with solution: ";
  print_vec(solution.solution);
  std::cout << std::endl;
 
  if(solution.value > needed_value)
  {
    std::cout << "Cannot make the needed value: " << needed_value << std::endl << std::endl;;

    /*
    for(type_value lesser_value = needed_value - 1; lesser_value > 0; --lesser_value)
    {
      std::cout << "Trying: " << lesser_value << std::endl;
      const auto lesser_value_reached =
        calc_optimal_sub_problem(coins, items_count, lesser_value ,
         sub_problems, sub_problems_solutions);
      if(lesser_value_reached == lesser_value)
      {
        std::cout << "lesser value_reached: " << value_reached << std::endl <<
          "with solution: ";
        print_vec(sub_problems_solutions[item_number][lesser_value_reached]);
        std::cout << std::endl;
      }
    }
    */

/*
    std::cout << "Coins attempted: ";
    print_vec(coins_used);
    std::cout << std::endl;
*/
  }
  else
  {
    /*
    std::cout << "Coins used for needed value: " << needed_value << ": ";
    print_vec(coins_used);
    std::cout << std::endl;
    */
  }

  return EXIT_SUCCESS;
}


