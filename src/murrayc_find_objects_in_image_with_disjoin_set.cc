// Build with:
// g++ -g -Wall -Werror -Wextra -Wpedantic -Wshadow --std=c++11 `pkg-config gdkmm-3.0 --libs --cflags` test.cc -o prog

#include <glibmm.h>
#include <gdkmm.h>
#include <gdkmm/wrap_init.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <climits>

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

guchar get_pixel(const guchar* pixels, int rowstride, int x, int y)
{
  auto p = pixels + (y * rowstride) + x;
  return *p;
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
  const auto pixels_size = width * height;
  const auto rowstride = pixbuf->get_rowstride();
  const auto rows_count = pixels_size / rowstride;

  //Iterate through every pixel in every row:
  //This is not the most efficient way, but it's clear:
  for(auto row = 0; row < rows_count; ++row)
  {
    auto row_start = pixels + (row * rowstride);
    for(auto i = 0; i < rowstride; ++i)
    {
      auto p = row_start + i;
      auto pixel = *p;
      if(pixel > THRESHHOLD)
      {
        std::cout << "pixel: " << (guint)pixel << std::endl;
      }
    }
  }

  return EXIT_SUCCESS;
}


