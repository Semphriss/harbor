//  SuperTux
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "util/rect.hpp"

#include <algorithm>

#include "util/size.hpp"
#include "util/vector.hpp"

Rect::Rect() :
  x1(),
  y1(),
  x2(),
  y2()
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
