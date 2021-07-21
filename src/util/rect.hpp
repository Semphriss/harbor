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

#ifndef _HEADER_HARBOR_UTIL_RECT_HPP
#define _HEADER_HARBOR_UTIL_RECT_HPP

class Size;
class Vector;

class Rect final
{
public:
  Rect();
  Rect(const Size& size);
  Rect(const Vector& top_left, const Size& size);
  Rect(float _x1, float _y1, float _x2, float _y2);

  Rect& move(const Vector& v);

  float width() const;
  float height() const;
  Vector top_lft() const;
  Vector top_rgt() const;
  Vector bot_lft() const;
  Vector bot_rgt() const;
  Vector mid() const;

  Rect grown(float f) const;
  Rect moved(const Vector& v) const;

  Rect operator*(const Size& s) const;

public:
  float x1, y1, x2, y2;
};

#endif
