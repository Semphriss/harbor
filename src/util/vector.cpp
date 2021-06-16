//  Harbor - A portable and highly customisable game engine
//  Copyright (C) 2021 Semphris <semphris@protonmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "util/vector.hpp"

#include "util/size.hpp"

Vector::Vector() :
  x(),
  y()
{
}

Vector::Vector(float _x, float _y) :
  x(_x),
  y(_y)
{
}

Vector::Vector(const Size& s) :
  x(s.w),
  y(s.h)
{
}

Size
Vector::size() const
{
  return Size(*this);
}

Vector
Vector::operator*(const Vector& v) const
{
  return Vector(x * v.x, y * v.y);
}

Vector
Vector::operator*(float f) const
{
  return Vector(x * f, y * f);
}

Vector
Vector::operator/(float f) const
{
  return Vector(x / f, y / f);
}

Vector&
Vector::operator*=(const Vector& v)
{
  x *= v.x;
  y *= v.y;
  return *this;
}

Vector
Vector::operator+(const Vector& v) const
{
  return Vector(x + v.x, y + v.y);
}

Vector&
Vector::operator+=(const Vector& v)
{
  x += v.x;
  y += v.y;
  return *this;
}

Vector
Vector::operator-(const Vector& v) const
{
  return Vector(x - v.x, y - v.y);
}

Vector&
Vector::operator-=(const Vector& v)
{
  x -= v.x;
  y -= v.y;
  return *this;
}

Vector
Vector::operator-() const
{
  return Vector(-x, -y);
}

bool
Vector::operator==(const Vector& v) const
{
  return x == v.x && y == v.y;
}

bool
Vector::operator!=(const Vector& v) const
{
  return x != v.x || y != v.y;
}

std::ostream&
operator<<(std::ostream& out, const Vector& v)
{
  out << "Vector(" << v.x << ", " << v.y << ")";
  return out;
}
