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

#include <memory>
#if __cplusplus <= 201103L

namespace std {

template<typename T, typename... A>
std::unique_ptr<T>
make_unique(A&&... args)
{
  return unique_ptr<T>(new T(std::forward<A>(args)...));
}

// make_shared, unlike make_unique, is already defined in C++11

}

#endif

/* EOF */
