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

#include "util/math.hpp"

#include <cmath>

#ifndef M_PI
#define M_PI 3.1415926535898
#endif

Vector
Math::rotate(const Vector& point, const Vector& around, float angle)
{
  angle *= M_PI / 180.f;
  // Source: https://web.archive.org/web/20190822004111/https://stackoverflow.co
  //         m/questions/22491178/how-to-rotate-a-point-around-another-point
  double x1 = point.x - around.x;
  double y1 = point.y - around.y;

  double x2 = x1 * std::cos(angle) - y1 * std::sin(angle);
  double y2 = x1 * std::sin(angle) + y1 * std::cos(angle);

  return Vector(x2 + around.x, y2 + around.y);
}

#else

template<class T> T
Math::clamp(const T& min, const T& max, const T& val)
{
  return (val < min) ? min : ((val > max) ? max : val);
}

#endif
