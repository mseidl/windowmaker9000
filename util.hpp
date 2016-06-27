#ifndef UTIL_HPP
#define UTIL_HPP

extern "C" {
#include  X11/Xlib.h
}
#include <string>

template<typename T>
struct Size
{
  T width;
  T height;

  Size() = default;
  Size(T w, T h) : width(w), height(h) { }

  std::string ToString() const;
};

template <typename T>
struct Position
{
  T x;
  T y;

  Position() = default;
  Position(T _x, T _y) : x(_x), y(_y) { }

  std::string ToString() const;
};

template <typename T>
struct Vector2D
{
  T x;
  T y;

  Vector2D() = default;
  Vector2D(T _x, T _y) : x(_x), y(_y) { }
   
  std::string ToString() const;
};

#endif // UTIL_HPP
