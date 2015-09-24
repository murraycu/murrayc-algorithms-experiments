// Build with:
// g++ -g -Wall -Werror -Wextra -Wpedantic -Wshadow --std=c++11 `pkg-config gdkmm-3.0 --libs --cflags` test.cc -o prog

#include <glibmm.h>
#include <gdkmm.h>
#include <gdkmm/wrap_init.h>

//Boost:
#include "disjoint_sets.hpp"

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
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
void print_vec_range(const std::vector<int>& vec, type_index start, type_index end)
{
  for(type_index i = start; i < end; ++i) {
    std::cout << vec[i] << ", ";
  }
}
*/


template<typename T>
constexpr const T&
as_const(T &t) noexcept
{ return t; }

template<typename T>
class comparator_greater
{
public:
  bool operator()(const T& a,const T& b) const
  {
    return a > b;
  }
};

/**
 * rowstride may be greater than the width,
 * if there is padding at the end of each row.
 */
static int get_pixel_pos(int width, int x, int y)
{
  if(x > width)
  {
    std::cerr << "Error: x > rowstride." << std::endl;
  }

  return (y * width) + x;
}

template <typename DistinctSets>
static
void first_pass(const guchar* pixels, int pixels_size, int width, int rowstride, int n_channels, DistinctSets& ds)
{
  std::cout << "first_pass():" << std::endl;

  const std::vector<std::pair<int, int>> neighbour_offsets =
  { {-1, 0}, //The pixel to the left.
    {-1, -1}, //The pixel to the above left.
    {0, -1}, //The pixel above.
    {1, -1} }; //The pixel above and to the right.

  //Iterate through every pixel in every row:
  //This is not the most efficient way, but it's clear:
  const auto rows_count = pixels_size / width;
  std::cout << "debug: rows_count=" << rows_count << std::endl;
  for(auto y = 0; y < rows_count; ++y)
  {
    auto pos_row = (y * width);
    for(auto x = 0; x < width; ++x)
    {
      const auto pos = pos_row + x;

      const auto p = pixels + (y * rowstride) + (x * n_channels);
      const auto pixel = *(p);
      std::cout << std::setw(3) << (int)pixel;

      if(pixel > THRESHHOLD)
      {
        std::cout << "(0)";
      }
      else
      {
        std::cout << "(1)";
        bool neighbour_found = false;
        //Check the neighbours:
        for(const auto offset : neighbour_offsets)
        {
          auto neighbour_x = x + offset.first;
          auto neighbour_y = y + offset.second;

          if((neighbour_x < width)
            && (neighbour_x > 0)
            && (neighbour_y < rows_count)
            && (neighbour_y > 0) )
          {
            auto neighbour_pos = get_pixel_pos(width,
              x + offset.first,
              y + offset.second);
            if(neighbour_pos > pixels_size)
            {
              std::cerr << "End of pixels buffer: pos=" << pos << ", neighbour_pos=" << neighbour_pos << ", pixels_size=" << pixels_size <<  std::endl;
              continue;
            }

            //If a "labelled" (in the ds/unionfind) neighbour was found,
            //use the same label for the current position,
            //by doing a union in the ds/unionfind.
            if(ds.find_set(neighbour_pos))
            {
              ds.union_set(pos, neighbour_pos);
              neighbour_found = true;

              //std::cout << "neighbour_found." << std::endl;
              break;
            }  
          }
        }

        //If this seems to be the start of a new object,
        //"label" it with a new label, in the ds/unionfind:
        if(!neighbour_found)
        {
          ds.make_set(pos);
          //std::cout << "new label." << std::endl;
        }
      }

      std::cout << std::setw(3) << ds.find_set(pos);

      std::cout << ", ";
    }

    std::cout << std::endl;
  }
}

template <typename DistinctSets>
void second_pass(int width, int height, DistinctSets& ds)
{
  std::cout << "second_pass():" << std::endl;
  std::set<int> roots;

  //TODO: Find a way to use boost::distinct_set::count_sets(begin, end) to make this easier:
  for(int y = 0; y < height; ++y)
  {
    for(int x = 0; x < width; ++x)
    {
      const auto pos = get_pixel_pos(width, x, y);
      const auto root = ds.find_set(pos);

      std::cout << std::setw(3) << root;
      if(root) //TODO: What happens if it's not found?
      {
        roots.insert(root);
      }

      std::cout << ", ";
    }

    std::cout << std::endl;
  }

  std::cout << "count: " << roots.size() << std::endl;
}

int main(int argc, char** argv)
{
  //TODO: gdkmm really needs a simple init() method, like gtkmm.
  Glib::init();
  //Gio::init();
  Gdk::wrap_init();

  if(argc < 2) {
    std::cerr << "Usage: theprogram theimagefile" << std::endl;
    return EXIT_FAILURE;
  }

  const char* filepath = argv[1];
  std::cout << "Loading file: " << filepath << std::endl;

  Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(filepath);
  if(!pixbuf)
  {
    std::cerr << "Could not load the pixbuf." << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Processing file: " << filepath << std::endl;
  //const auto pixels = pixbuf->get_pixels(); //TODO: Wrap the _with_length() version in gdkmm.
  guint pixels_length = 0;
  const auto pixels = gdk_pixbuf_get_pixels_with_length(pixbuf->gobj(), &pixels_length);

  const auto width = pixbuf->get_width();
  const auto height = pixbuf->get_height();
  const auto rowstride = pixbuf->get_rowstride();
  const auto n_channels = pixbuf->get_n_channels(); //bytes per pixel.
  std::cout << "debug: width=" << width << ", height=" << height << ", rowstride=" << rowstride << std::endl;
  const auto pixels_size = width * height;
  //const auto pixels_end = pixels + pixels_size;



  std::vector<int> rank;
  rank.resize(pixels_size);
  std::vector<int> parent;
  parent.resize(pixels_size);
  boost::disjoint_sets<int*, int*> ds(&rank[0], &parent[0]);

  first_pass(pixels, pixels_size, width, rowstride, n_channels, ds);
  second_pass(width, height, ds);

  return EXIT_SUCCESS;
}


