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

#ifndef _HEADER_HARBOR_UTIL_VECTOR_HPP
#define _HEADER_HARBOR_UTIL_VECTOR_HPP

#include <cmath>
#include <ostream>

class Size;

class Vector final
{
public:
  Vector();
  Vector(float _x, float _y);
  Vector(const Size& s);

  Size size() const;
  float length() const;

  Vector floor() const;
  Vector ceil() const;

  Vector operator*(const Vector& v) const;
  Vector operator*(float f) const;
  Vector& operator*=(const Vector& s);
  Vector& operator*=(float f);
  Vector operator/(const Vector& v) const;
  Vector operator/(float f) const;
  Vector& operator/=(const Vector& s);
  Vector& operator/=(float f);
  Vector operator+(const Vector& v) const;
  Vector& operator+=(const Vector& v);
  Vector operator-(const Vector& v) const;
  Vector& operator-=(const Vector& v);
  Vector operator-() const;
  Vector operator%(float f) const;
  bool operator==(const Vector& v) const;
  bool operator!=(const Vector& v) const;

  friend std::ostream& operator<<(std::ostream& out, const Vector& v);

public:
  float x, y;
};

#endif
