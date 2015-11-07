#include <iostream>
#include <string>
#include <cstdlib>

typedef std::string Container;
typedef Container::value_type Element;

int choose_pivot(Container& elements, int low, int high)
{
  std::swap(elements[low], elements[high]);
  return low;
}

int partition(Container& elements, int low, int high) {
  if(low >= high)
    return low;

  std::cout << std::endl << "partition(): low=" << low << ", high=" << high << std::endl;
  std::cout << "debug before partition: " << elements.substr(low, high - low + 1) << std::endl;
  //Choose partition and put it in the first position:
  const int p = choose_pivot(elements, low, high);

  std::cout << "debug after swap to start: " << elements.substr(low, high - low + 1) << std::endl;
  const auto pivot_value = elements[low];
  std::cout << "partition value: " << pivot_value << std::endl;

  int j = low;
  for (int i = low + 1; i <= high; ++i) {
    //std::cout << "i: " << i << std::endl;
    const auto& value = elements[i];
    //std::cout << "comparison: value=" << value << value << ", partition=" << pivot_value << std::endl;

    //We MUST use <=. Using < means that equal values will not be moved next to each other.
    if (value <= pivot_value) {
      //std::cout << "swapping: " << elements[j] << " and " << elements[i] << std::endl;
      ++j;
      std::swap(elements[j], elements[i]);
    }
  }

  std::swap(elements[p], elements[j]);

  std::cout << "debug after partition: " << elements.substr(low, high - low + 1) << std::endl;
  std::cout << "partition(): returning j=" << j << std::endl << std::endl;
  return j;
}

void quicksort(Container& elements, int low, int high) {
  std::cout << std::endl << "quicksort(): low=" << low << ", high=" << high << std::endl << std::endl;
  //std::cout << "debug before: " << elements.substr(low, high - low + 1) << std::endl;

  const int p = partition(elements, low, high);

  //std::cout << "debug after: " << elements.substr(low, high - low + 1) << std::endl;

  if (p < 0)
    return; //Not found.

  if (p > high)
    return; //Not found.
  
  quicksort(elements, low, p - 1);
  quicksort(elements, p + 1, high);
}

void quicksort(Container& elements) {
  const auto size = elements.size();
  if (size == 0) {
    return;
  }

  quicksort(elements, 0, elements.size() - 1);
}

int main() {
  Container str = "974563481";

  std::cout << "start: " << str << std::endl;

  //const int k = 2; //0-indexed
  quicksort(str);
  std::cout << "sortd: " << str << std::endl;
  return EXIT_SUCCESS;
}
