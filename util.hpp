#ifndef UTIL_HPP
#define UTIL_HPP

extern "C" 
{
  #include  <X11/Xlib.h>
}

#include <string>
#include <sstream>
#include <ostream>
#include <vector>
#include <algorithm>

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

template <typename T>
std::string Vector2D<T>::ToString() const
{
  std::ostringstream out;
  out << '(' << x << ", " << y << ')';
  return out.str();
}

template<typename T>
std::ostream& operator<< (std::ostream& out, Vector2D<T>& vec)
{
  return out << vec.ToString();
}

template<typename T>
std::ostream& operator<< (std::ostream& out, const Size<T>& size)
{
  return out << size.ToString();
}

template <typename T>
std::ostream& operator<< (std::ostream& out, Position<T>& pos)
{
  return out << pos.ToString();
}

template<typename T>
Vector2D<T> operator- (const Position<T>& lhs, const Position<T>& rhs)
{
  return Vector2D<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}

template<typename T>
Position<T> operator- (const Vector2D<T>& lhs, const Position<T>& rhs)
{
  return Vector2D<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}

template<typename T>
Position<T> operator+ (const Position<T>& lhs, const Vector2D<T>& rhs)
{
  return Position<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}

template<typename T>
Position<T> operator+ (const Vector2D<T>& lhs, const Position<T>& rhs)
{
  return Position<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}

template<typename T>
Vector2D<T> operator- (const Size<T>& lhs, const Size<T>& rhs)
{
  return Vector2D<T>(lhs.width - rhs.width, lhs.height - rhs.height);
}

template<typename T>
Size<T> operator+ (const Size<T>& lhs, const Vector2D<T>& rhs)
{
  return Size<T>(lhs.width + rhs.x, lhs.height + rhs.y);
}

template<typename T>
Size<T> operator+ (const Vector2D<T>& lhs, const Size<T>& rhs)
{
  return Size<T>(lhs.x + rhs.width, lhs.y + rhs.height);
}

template<typename T>
Size<T> operator- (const Size<T>& lhs, const Vector2D<T>& rhs)
{
  return Size<T>(lhs.width - rhs.x, lhs.height - rhs.y);
}

extern std::string ToString(const XEvent* xev);

extern std::string XConfigureWindowValueMaskToString(unsigned long value_mask);

extern std::string XRequestCodeToString(unsigned char request_code);

template<typename T>
std::string ToString(const T& x)
{
  std::ostringstream out;
  out << x;
  return out.str();
}

template<typename Container>
std::string Join(const Container& cont, const std::string delim)
{
  std::ostringstream out;
  for (auto i = cont.cbegin(); i != cont.cend(); ++i)
  {
    if (i != cont.cbegin())
    {
      out << delim;
    }
    out << *i;
  }

  return out.str();
}

template<typename Container, typename Converter>
std::string Join(const Container& cont, const std::string& delim, Converter conv)
{
  std::vector<std::string> converted_container(cont.size());
  std::transform(cont.cbegin(), cont.cend(), converted_container.begin(), conv);
  return Join(converted_container, delim);
}

#endif // UTIL_HPP
