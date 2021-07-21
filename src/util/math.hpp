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

#ifndef _HEADER_HARBOR_UTIL_MATH_HPP
#define _HEADER_HARBOR_UTIL_MATH_HPP

#include "util/vector.hpp"

class Math final
{
public:
  template<class T> static T clamp(const T& min, const T& max, const T& val);
  static Vector rotate(const Vector& point, const Vector& around, float angle);
};

#include "util/math.cpp"

#endif
