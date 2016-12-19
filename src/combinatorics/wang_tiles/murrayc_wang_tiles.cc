#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <boost/timer/timer.hpp>

class Card {
public:
  enum Color {
    RED,
    GREEN,
    BLUE,
    YELLOW
    // GREY
  };

  Card(Color top, Color right, Color bottom, Color left)
  : edges_{{top, right, bottom, left}} {}

  static constexpr unsigned int EDGES_COUNT = 4;
  using edges_type = std::array<Color, EDGES_COUNT>;

  edges_type
  get_edges(unsigned int rotation) const {
    auto result = edges_;
    // Rotate to the right:
    std::rotate(
      std::rbegin(result), std::rbegin(result) + rotation, std::rend(result));
    return result;
  }

  static constexpr unsigned int EDGE_TOP = 0;
  static constexpr unsigned int EDGE_RIGHT = 1;
  static constexpr unsigned int EDGE_BOTTOM = 2;
  static constexpr unsigned int EDGE_LEFT = 3;

public:
  edges_type edges_;
};

using card_and_rotation = std::pair<unsigned int, unsigned int>;
using solution_type = std::vector<card_and_rotation>;

static std::string
print_color(Card::Color color) {
  switch (color) {
    case Card::RED:
      return "R";
    case Card::GREEN:
      return "G";
    case Card::BLUE:
      return "B";
    case Card::YELLOW:
      return "Y";
      // case Card::GREY:
      //  return "K";
  }

  return "?";
}

class Grid {
public:
  Grid(unsigned int width, unsigned int height)
  : width_(width), height_(height) {}

  unsigned int
  get_width() const {
    return width_;
  }

  unsigned int
  get_height() const {
    return height_;
  }

  unsigned int
  get_cells_count() const {
    return width_ * height_;
  }

  using cards_type = std::vector<Card>;

  bool
  next_card_satisfies_constraints(const cards_type& cards,
    const solution_type& prefix, std::size_t prefix_size,
    const Card& card, unsigned int rotation) const {
    const auto card_pos = prefix_size; // 1 later.
    // std::cout << "  testing: card_pos=" << card_pos << ", rotation=" <<
    // rotation << '\n';
    for (unsigned int edge = 0; edge < Card::EDGES_COUNT; ++edge) {
      // std::cout << "edge: " << edge << '\n';
      unsigned int neighbour_pos = 0;
      if (!get_neighbour_pos(card_pos, edge, neighbour_pos)) {
        // There is no neighbouring edge here
        // (it must be the edge of the grid),
        // so this edge cannot fail.
        // std::cout << "      no edge.\n";
        continue;
      }

      if (neighbour_pos >= card_pos) {
        // There is no card in this edge's neighbouring position yet,
        // so this edge cannot fail.
        // std::cout << "      no neighbour.\n";
        continue;
      }

      // std::cout << "      neighbour_pos: " << neighbour_pos << '\n';
      const auto neighbour = prefix[neighbour_pos];
      // std::cout << "        neighbour card: " << neighbour.first << '\n';
      const auto& neighbour_card = cards[neighbour.first];
      auto neighbour_edge = get_neighbour_edge(edge);
      neighbour_edge = rotated_edge(neighbour_edge, neighbour.second);
      const auto card_edge = rotated_edge(edge, rotation);
      // std::cout << "        card edge=" << edge << ", with rotation=" <<
      // rotation << ": " << card_edge << "\n";
      if (!edges_match(neighbour_card, neighbour_edge, card, card_edge)) {
        //std::cout << "        conflict.\n";
        return false;
      }

      // std::cout << "        edges match: current: " <<
      // print_color(card.edges_[card_edge])
      //  << ", neighbour_edge: " <<
      //  print_color(neighbour_card.edges_[neighbour_edge]) << '\n';
    }

    // std::cout << "    no conflict.\n";
    return true;
  }

private:
  /** From 0 to 3, meaning top, right, bottom, and left.
   */
  using edge_type = unsigned int;

  /** Get the neighbours edge that matches with the cell's edge.
   */
  static edge_type
  get_neighbour_edge(edge_type edge) {
    switch (edge) {
      case Card::EDGE_TOP:
        return Card::EDGE_BOTTOM;
      case Card::EDGE_RIGHT:
        return Card::EDGE_LEFT;
      case Card::EDGE_BOTTOM:
        return Card::EDGE_TOP;
      case Card::EDGE_LEFT:
        return Card::EDGE_RIGHT;
    }

    std::cerr << "Unexpected edge type: " << edge << '\n';
    return 0; // Should not happen.
  }

  /** Get the original edge index for the @a edge
   * if it is rotated. For instance, if the edge is
   * rotated right by 1, then getting edge 1 will
   * actually get original edge 0.
   */
  static edge_type
  rotated_edge(edge_type edge, unsigned int rotation) {
    edge_type result = edge + (Card::EDGES_COUNT - rotation);
    if (result >= Card::EDGES_COUNT) {
      result -= Card::EDGES_COUNT;
    }
    assert(result < Card::EDGES_COUNT);

    return result;
  }

  bool
  has_edge(unsigned int pos, edge_type edge) const {
    const auto x = pos % width_;
    const auto y =
      pos /
      height_; // Most compilers will combine the % and / into 1 instruction.
    return has_edge(x, y, edge);
  }

  /** @param edge From 0 to 3, meaning top, right, bottom, and left.
   */
  bool
  has_edge(unsigned int x, unsigned int y, edge_type edge) const {
    assert(edge <= Card::EDGES_COUNT);

    if (edge == Card::EDGE_TOP) {
      return !(y == 0);
    } else if (edge == Card::EDGE_RIGHT) {
      return !(x == (width_ - 1));
    } else if (edge == Card::EDGE_BOTTOM) {
      return !(y == (height_ - 1));
    } else if (edge == Card::EDGE_LEFT) {
      return !(x == 0);
    }

    std::cerr << "Unexpected edge type: " << edge << '\n';
    return false; // Should not happen.
  }

  bool
  get_neighbour_pos(
    unsigned int pos, edge_type edge, unsigned int& neighbour_pos) const {
    assert(edge <= Card::EDGES_COUNT);
    neighbour_pos = 0;

    if (!has_edge(pos, edge)) {
      return false;
    }

    if (edge == Card::EDGE_TOP) {
      neighbour_pos = pos - width_;
      return true;
    } else if (edge == Card::EDGE_RIGHT) {
      neighbour_pos = pos + 1;
      return true;
    } else if (edge == Card::EDGE_BOTTOM) {
      neighbour_pos = pos + width_;
      return true;
    } else if (edge == Card::EDGE_LEFT) {
      neighbour_pos = pos - 1;
      return true;
    }

    std::cerr << "Unexpected edge type: " << edge << '\n';
    return false; // Should not happen.
  }

  static bool
  edges_match(const Card& card_a, unsigned int card_a_edge, const Card& card_b,
    unsigned int card_b_edge) {
    return card_a.edges_[card_a_edge] == card_b.edges_[card_b_edge];
  }

  unsigned int width_, height_;
};

static void
print_solution(const solution_type& solution, std::size_t solution_size, const Grid& grid,
  const Grid::cards_type& cards) {
  const auto width = grid.get_width();
  const auto height = grid.get_height();
  const auto cards_count = grid.get_cells_count();
  const auto placement_count = solution_size;

  for (std::size_t i = 0; i < solution_size; ++i) {
    const auto& p = solution[i];
    std::cout << "(" << p.first << ", " << p.second << ") ";
  }
  std::cout << std::endl;

  for (unsigned int y = 0; y < height; ++y) {
    // Print the top, the middle, and then the bottom:
    for (unsigned int part = 0; part < 3; ++part) {
      for (unsigned int x = 0; x < width; ++x) {
        const auto i = y * width + x;
        if (i >= placement_count) {
          continue;
        }

        const auto& placement = solution[i];

        const auto card_index = placement.first;
        const auto rotation = placement.second;

        if (card_index >= cards_count) {
          std::cerr << "Unexpected card index " << card_index << " (count: " << cards_count << ") in solution: " << card_index
                    << '\n';
          continue;
        }

        const auto& card = cards[card_index];
        const auto edges = card.get_edges(rotation);
        if (part == 0) {
          std::cout << " " << print_color(edges[Card::EDGE_TOP]) << " ";
        } else if (part == 1) {
          std::cout << print_color(edges[Card::EDGE_LEFT]) << " "
                    << print_color(edges[Card::EDGE_RIGHT]);
        } else if (part == 2) {
          std::cout << " " << print_color(edges[Card::EDGE_BOTTOM]) << " ";
        }

        std::cout << " ";
      }
      std::cout << "\n";
    }

    std::cout << "\n";
  }
}

static void
print_solution(const solution_type& solution, const Grid& grid,
  const Grid::cards_type& cards) {
  print_solution(solution, solution.size(), grid, cards);
}

static inline bool
contains_card(const solution_type& solution, std::size_t size,
  std::size_t card_index) {
  const auto end = std::cbegin(solution) + size;
  return std::find_if(std::cbegin(solution), end,
    [card_index](const auto& item) {
      return item.first == card_index;
    }) != end;
}

/**
 */
static std::vector<solution_type>
get_solutions(const Grid& grid, const Grid::cards_type& cards) {
  std::vector<solution_type> result;
  const auto solution_size_needed = grid.get_cells_count();

  solution_type solution(solution_size_needed);
  std::size_t i = 0;
  std::size_t try_card = 0;
  std::size_t try_rotation = 0;

  const auto cards_count = cards.size();

  while (true) {
    //std::cout << "LOOP: " << i << std::endl;
    //std::cout << "  card: " << try_card << ", rotation: " << try_rotation << std::endl;
    if (try_rotation >= Card::EDGES_COUNT) {
      ++try_card;
      try_rotation = 0;
    }

    if (try_card >= cards_count) {
      if (i == 0) {
        // We have tried all possibilities:
        break;
      } else {
        // Backtrack, to try an alternative choice for the previous step:
        //std::cout << "i = " << i << ": BACKTRACK" << std::endl;
        --i;
        const auto& prev = solution[i];
        try_card = prev.first;
        try_rotation = prev.second;
        ++try_rotation;
        continue;
      }
    }

    // Ignore already-used cards:
    if (contains_card(solution, i, try_card)) {
      ++try_card;
      try_rotation = 0;
      continue;
    }

    const auto& card = cards[try_card];
    if (grid.next_card_satisfies_constraints(cards, solution, i, card, try_rotation)) {
      //std::cout << "  Adding: i=" << i << ", card=" << try_card << ", rotation=" << try_rotation << std::endl;
      solution[i] = std::make_pair(try_card, try_rotation);

      //print_solution(solution, i+1, grid, cards);

      if (i == (solution_size_needed - 1)) {
        //std::cout << "USING solution." << std::endl;
        result.emplace_back(solution);
        // Continue, trying the next card/rotation.
      } else {
        ++i;
        try_card = 0;
        try_rotation = 0;
        continue;
      }
    }

    ++try_rotation;
  }

  return result;
}

int
main() {
  constexpr unsigned int WIDTH = 3;
  constexpr unsigned int HEIGHT = 3;
  constexpr auto COUNT = WIDTH * HEIGHT;

  Grid grid(WIDTH, HEIGHT);

  using Color = Card::Color;

  // See http://www.cr31.co.uk/stagecast/wang/3corn.html
  const Grid::cards_type cards = {
    {Color::RED, Color::GREEN, Color::YELLOW, Color::BLUE},
    {Color::GREEN, Color::BLUE, Color::YELLOW, Color::GREEN},
    {Color::GREEN, Color::BLUE, Color::GREEN, Color::RED},
    {Color::GREEN, Color::RED, Color::YELLOW, Color::YELLOW},
    {Color::BLUE, Color::YELLOW, Color::GREEN, Color::RED},
    {Color::RED, Color::GREEN, Color::YELLOW, Color::BLUE},
    {Color::GREEN, Color::YELLOW, Color::GREEN, Color::RED},
    {Color::GREEN, Color::BLUE, Color::RED, Color::YELLOW},
    {Color::BLUE, Color::YELLOW, Color::RED, Color::GREEN}};
  assert(cards.size() >= COUNT);


  {
    boost::timer::auto_cpu_timer timer;
    for (int i = 0; i < 50; i++) {
      const auto solutions = get_solutions(grid, cards);
      //std::cout << "solutions count: " << solutions.size() << std::endl;
      assert(!solutions.empty());
      assert(solutions.size() == 5472);
    }
  }

  const auto solutions = get_solutions(grid, cards);
  std::cout << "solutions count: " << solutions.size() << std::endl;
  for (int i = 0; i < 10; ++i) {
    std::cout << i << ":\n";
    const auto& solution = solutions[i];
    print_solution(solution, grid, cards);
  }

  return EXIT_SUCCESS;
}
