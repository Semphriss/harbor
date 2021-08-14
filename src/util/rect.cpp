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

#include "util/rect.hpp"

#include <algorithm>

#include "util/size.hpp"
#include "util/vector.hpp"

Rect::Rect() :
  x1(0.f),
  y1(0.f),
  x2(0.f),
  y2(0.f)
{
}

Rect::Rect(const Size& size) :
  x1(0.f),
  y1(0.f),
  x2(size.w),
  y2(size.h)
{
}

Rect::Rect(const Vector& top_left, const Size& size) :
  x1(top_left.x),
  y1(top_left.y),
  x2(top_left.x + size.w),
  y2(top_left.y + size.h)
{
}

Rect::Rect(float _x1, float _y1, float _x2, float _y2) :
  x1(_x1),
  y1(_y1),
  x2(_x2),
  y2(_y2)
{
}

Rect&
Rect::move(const Vector& v)
{
  x1 += v.x;
  x2 += v.x;
  y1 += v.y;
  y2 += v.y;
  return *this;
}

float
Rect::width() const
{
  return x2 - x1;
}

float
Rect::height() const
{
  return y2 - y1;
}

Vector
Rect::top_lft() const
{
  return Vector(x1, y1);
}

Vector
Rect::top_rgt() const
{
  return Vector(x2, y1);
}

Vector
Rect::bot_lft() const
{
  return Vector(x1, y2);
}

Vector
Rect::bot_rgt() const
{
  return Vector(x2, y2);
}

Vector
Rect::mid() const
{
  return Vector((x1 + x2) / 2.f, (y1 + y2) / 2.f);
}

bool
Rect::contains(const Vector& point) const
{
  return point.x >= x1 && point.x <= x2 && point.y >= y1 && point.y <= y2;
}

Rect
Rect::grown(float f) const
{
  return Rect(x1 - f, y1 - f, x2 + f, y2 + f);
}

Rect
Rect::moved(const Vector& v) const
{
  return Rect(Vector(x1, y1) - v, Size(width(), height()));
}

Rect
Rect::operator*(const Size& s) const
{
  return Rect(Vector(x1, y1), Size(width(), height()) * s);
}