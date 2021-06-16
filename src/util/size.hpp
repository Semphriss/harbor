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

#ifndef _HEADER_HARBOR_UTIL_SIZE_HPP
#define _HEADER_HARBOR_UTIL_SIZE_HPP

#include <ostream>

class Vector;

class Size final
{
public:
  Size();
  Size(float _w, float _h);
  Size(const Vector& v);

  Vector vector() const;

  Size operator*(const Size& s) const;
  Size operator*(float f) const;
  Size operator/(float f) const;
  Size operator/(const Size s) const;
  Size& operator*=(const Size& s);
  Size operator+(const Size& s) const;
  bool operator==(const Size& v) const;
  bool operator!=(const Size& v) const;

  friend std::ostream& operator<<(std::ostream& out, const Size& s);

public:
  float w, h;
};

#endif
