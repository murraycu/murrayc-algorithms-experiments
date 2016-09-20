#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

using matrix_type = std::vector<std::vector<unsigned int>>;

static matrix_type
create_matrix() {
  return {{0, 0}, {0, 0}};
}

static const auto&
matrix_xy(const matrix_type& matrix, unsigned int x, unsigned int y) {
  return matrix[y][x];
}
static auto&
matrix_xy(matrix_type& matrix, unsigned int x, unsigned int y) {
  return matrix[y][x];
}

static matrix_type
matrix_multiply(const matrix_type& a, const matrix_type& b) {
  matrix_type result = create_matrix();

  const auto a_width = 2;
  const auto result_width = 2; // b's height.
  const auto result_height = a_width;
  for (unsigned int result_x = 0; result_x < result_width; ++result_x) {
    for (unsigned int result_y = 0; result_y < result_height; ++result_y) {
      unsigned int val = 0;
      for (unsigned int a_x = 0; a_x < a_width; ++a_x) {
        // std::cout << result_x << ", " << result_y << " += " << matrix_xy(a,
        // a_x, result_y) << " * " << matrix_xy(b, result_x, a_x) << '\n';
        val += matrix_xy(a, a_x, result_y) * matrix_xy(b, result_x, a_x);
      }
      matrix_xy(result, result_x, result_y) = val;
    }
  }

  return result;
}

// This should be O(log n), due to the halving and recursing.
static matrix_type
matrix_pow(const matrix_type& matrix, unsigned int n) {
  if (n == 1) {
    return matrix;
  }

  if (n == 2) {
    // Do multiplication:
    return matrix_multiply(matrix, matrix);
  }

  // If n is even:
  if (n % 2 == 0) {
    const auto half_pow = matrix_pow(matrix, n / 2);
    return matrix_pow(half_pow, 2);
  }

  // If n is odd, call recursively, but with an even n:
  return matrix_multiply(matrix_pow(matrix, n - 1), matrix);
}

// This should be O(log n) because matrix_pow() should be O(log n).
static unsigned int
fibonacci(unsigned int n) {
  if (n == 0)
    return 0;
  else if (n == 1)
    return 1;

  // Based on Binet's formula.
  // See
  // https://www.reddit.com/r/cpp/comments/4lpdwz/c_weekly_ep13_fibonacci_youre_doing_it_wrong/
  const matrix_type matrix_A = {{0, 1}, {1, 1}};
  const auto A_to_n = matrix_pow(matrix_A, n - 1);
  return matrix_xy(A_to_n, 1, 1);
}

int
main() {
  matrix_type matrix_a = {{1, 2}, {7, 8}};
  matrix_type matrix_b = {{4, 5}, {9, 10}};
  const auto matrix_mult = matrix_multiply(matrix_a, matrix_b);
  // std::cout << "row0: " << matrix_xy(matrix_mult, 0, 0) << ", " <<
  // matrix_xy(matrix_mult, 1, 0) << '\n';
  // std::cout << "row1: " << matrix_xy(matrix_mult, 0, 1) << ", " <<
  // matrix_xy(matrix_mult, 1, 1) << '\n';
  assert(matrix_xy(matrix_mult, 0, 0) == 22);
  assert(matrix_xy(matrix_mult, 1, 0) == 25);
  assert(matrix_xy(matrix_mult, 0, 1) == 100);
  assert(matrix_xy(matrix_mult, 1, 1) == 115);

  assert(fibonacci(1) == 1);
  assert(fibonacci(2) == 1);
  assert(fibonacci(3) == 2);
  assert(fibonacci(4) == 3);
  assert(fibonacci(5) == 5);
  assert(fibonacci(6) == 8);
  assert(fibonacci(7) == 13);
  assert(fibonacci(8) == 21);
  assert(fibonacci(9) == 34);
  assert(fibonacci(10) == 55);
  assert(fibonacci(40) == 102334155);
  return EXIT_SUCCESS;
}
