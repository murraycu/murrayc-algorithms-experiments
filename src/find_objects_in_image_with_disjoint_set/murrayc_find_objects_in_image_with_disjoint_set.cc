// Build with:
// g++ -g -Wall -Werror -Wextra -Wpedantic -Wshadow --std=c++11 `pkg-config
// gdkmm-3.0 --libs --cflags` test.cc -o prog

#include <gdkmm.h>
#include <gdkmm/wrap_init.h>
#include <glibmm.h>

// Boost:
#include "disjoint_sets.hpp"

//#include "union_find.h"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>

const guint THRESHHOLD = 100;

/*
static
void indent(type_index level)
{
  std::cout << "level: " << level;
  for(type_index l = 0; l < level; ++l) {
    std::cout << "  ";
  }
}

static
void print_vec(const std::vector<int>& vec)
{
 for(auto num : vec) {
    std::cout << num << ", ";
  }
}

static
void print_vec_range(const std::vector<int>& vec, type_index start, type_index
end)
{
  for(type_index i = start; i < end; ++i) {
    std::cout << vec[i] << ", ";
  }
}
*/

template <typename T>
constexpr const T&
as_const(T& t) noexcept {
  return t;
}

/**
 * rowstride may be greater than the width,
 * if there is padding at the end of each row.
 */
static int
get_pixel_pos(int width, int x, int y) {
  if (x > width) {
    std::cerr << "Error: x > rowstride." << std::endl;
  }

  return (y * width) + x;
}

/**
 * Returns the number of labels set.
 */
template <typename DistinctSets>
static int
first_pass(const guchar* pixels, int pixels_size, int width, int rowstride,
  int n_channels, std::vector<int>& pixels_labelled, DistinctSets& ds) {
  std::cout << "first_pass():" << std::endl;

  int next_label = 1;

  const std::vector<std::pair<int, int>> neighbour_offsets = {
    {-1, 0},  // The pixel to the left.
    {-1, -1}, // The pixel to the above left.
    {0, -1},  // The pixel above.
    {1, -1}}; // The pixel above and to the right.

  // Iterate through every pixel in every row:
  // This is not the most efficient way, but it's clear:
  const auto rows_count = pixels_size / width;
  std::cout << "debug: rows_count=" << rows_count << std::endl;
  for (auto y = 0; y < rows_count; ++y) {
    auto pos_row = (y * width);
    for (auto x = 0; x < width; ++x) {
      const auto pos = pos_row + x;

      const auto p = pixels + (y * rowstride) + (x * n_channels);
      const auto pixel = *(p);
      // std::cout << "pos: " << pos << ":  ";
      std::cout << std::setw(3) << (int)pixel;

      if (pixel > THRESHHOLD) {
        std::cout << "(0) ";
      } else {
        std::cout << "(1)";
        bool neighbour_found = false;
        // Check the neighbours:
        for (const auto offset : neighbour_offsets) {
          auto neighbour_x = x + offset.first;
          auto neighbour_y = y + offset.second;

          if ((neighbour_x < width) && (neighbour_x >= 0) &&
              (neighbour_y < rows_count) && (neighbour_y >= 0)) {
            auto neighbour_pos =
              get_pixel_pos(width, x + offset.first, y + offset.second);
            if (neighbour_pos > pixels_size) {
              std::cerr << "End of pixels buffer: pos=" << pos
                        << ", neighbour_pos=" << neighbour_pos
                        << ", pixels_size=" << pixels_size << std::endl;
              continue;
            }

            // If a "labelled" neighbour was found,
            // use the lowest neighbour label for the current position:
            const auto pos_label = pixels_labelled[pos];
            const auto neighbour_label = pixels_labelled[neighbour_pos];
            // std::cout << "pos(" << pos << "): root(" << neighbour_pos << "):
            // " << root << std::endl;
            if (neighbour_label) {
              if (pos_label && (neighbour_label > pos_label)) {
                // std::cout << " union_set(" << pos_label << ", " <<
                // neighbour_label;
                ds.union_set(pos_label, neighbour_label);
                continue;
              }

              // Use the neighbour label if it is the lowest neighbour label
              // found so far:
              // std::cout << " neighbour label:" << neighbour_label;
              pixels_labelled[pos] = neighbour_label;
              neighbour_found = true;
            }
          }
        }

        // If this seems to be the start of a new object,
        //"label" it with a new label, in the ds/unionfind:
        if (!neighbour_found) {
          pixels_labelled[pos] = next_label;
          ds.make_set(next_label);

          // std::cout << "new label:" << next_label << std::endl;

          next_label++;
        }

        std::cout << (neighbour_found ? "n" : " ");
      }

      std::cout << std::setw(3) << pixels_labelled[pos];

      std::cout << ", ";
      // std::cout << std::endl;
    }

    std::cout << std::endl;
  }

  return next_label - 1;
}

template <typename DistinctSets>
void
second_pass(int labels_count, DistinctSets& ds) {
  std::cout << "second_pass():" << std::endl;
  std::set<int> roots;

  // TODO: Find a way to use: boost::distinct_set::count_sets(begin, end) to
  // make this easier:

  for (int label = 1; label <= labels_count; ++label) {
    const auto root = ds.find_set(label);

    std::cout << std::setw(3) << root;
    roots.insert(root);

    std::cout << ", ";
  }

  std::cout << "count: " << roots.size() << std::endl;
}

int
main(int argc, char** argv) {
  // TODO: gdkmm really needs a simple init() method, like gtkmm.
  Glib::init();
  // Gio::init();
  Gdk::wrap_init();

  const char* filepath = nullptr;

  if (argc < 2) {
    std::cerr << "Usage: theprogram theimagefile" << std::endl;
    // return EXIT_FAILURE;

    // This is just so we have something to do during "make check":
    std::cerr << "  Using test.png by default." << std::endl;
    filepath = "src/find_objects_in_image_with_disjoint_set/test.png";
  } else {
    filepath = argv[1];
  }

  std::cout << "Loading file: " << filepath << std::endl;

  Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(filepath);
  if (!pixbuf) {
    std::cerr << "Could not load the pixbuf." << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Processing file: " << filepath << std::endl;
  // const auto pixels = pixbuf->get_pixels(); //TODO: Wrap the _with_length()
  // version in gdkmm.
  guint pixels_length = 0;
  const auto pixels =
    gdk_pixbuf_get_pixels_with_length(pixbuf->gobj(), &pixels_length);

  const auto width = pixbuf->get_width();
  const auto height = pixbuf->get_height();
  const auto rowstride = pixbuf->get_rowstride();
  const auto n_channels = pixbuf->get_n_channels(); // bytes per pixel.
  std::cout << "debug: width=" << width << ", height=" << height
            << ", rowstride=" << rowstride << std::endl;
  const auto pixels_size = width * height;
  // const auto pixels_end = pixels + pixels_size;

  const auto pos_count = width * height;
  std::vector<int> rank;
  rank.resize(pos_count);
  std::vector<int> parent;
  parent.resize(pos_count);
  boost::disjoint_sets<int*, int*> ds(&rank[0], &parent[0]);
  // UnionFind<int> ds(pos_count);

  std::vector<int> pixels_labelled;
  pixels_labelled.resize(pos_count);
  const auto labels_count = first_pass(
    pixels, pixels_size, width, rowstride, n_channels, pixels_labelled, ds);

  second_pass(labels_count, ds);

  return EXIT_SUCCESS;
}
