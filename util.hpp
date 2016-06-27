#ifndef UTIL_HPP
#define UTIL_HPP

extern "C" {
#include  X11/Xlib.h
}
#include <string>
#include <sstream>
#include <ostream>

template<typename T>
struct Size
{
  T width;
  T height;

  Size() = default;
  Size(T w, T h) : width(w), height(h) { }

  std::string ToString() const;
};

template<typename T>
std::ostream& operator<< (std::ostream& out, const Size<T>& size)
{
  return out << size.ToString();
}

template<typename T>
Size<T> operator- (const 

template<typename T>
std::string Size<T>::ToString() const
{
  std::ostringstream out;
  out << width << 'x' << height;
  return out.str();
}

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
std::ostream& operator<< (std::ostream& out, Position<T>& pos)
{
  return out << pos.ToString();
}

template <typename T>
std::string Position<T>::ToString() const
{
  std::ostringstream out;
  out << '(' << x << ", " << y << ')';
  return out.str();
}

template <typename T>
struct Vector2D
{
  T x;
  T y;

  Vector2D() = default;
  Vector2D(T _x, T _y) : x(_x), y(_y) { }
   
  std::string ToString() const;
};

template<typename T>
std::ostream& operator<< (std::ostream& out, Vector2D<T>& vec)
{
  return out << vec.ToString();
}

template <typename T>
std::string Vector2D<T>::ToString() const
{
  std::ostringstream out;
  out << '(' << x << ", " << y << ')';
  return out.str();
}

#endif // UTIL_HPP
