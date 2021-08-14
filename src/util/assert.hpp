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

#ifndef _HEADER_HARBOR_UTIL_ASSERT_HPP
#define _HEADER_HARBOR_UTIL_ASSERT_HPP

#include "util/log.hpp"

#define assert_(COND, MSG, LEVEL) { if (!(COND)) Log::log(#LEVEL, __FILE__,    \
                                  __LINE__) << MSG << std::endl; }
#define assert_debug(COND, MSG) assert_(COND, MSG, DEBUG)
#define assert_info(COND, MSG)  assert_(COND, MSG, INFO)
#define assert_warn(COND, MSG)  assert_(COND, MSG, WARN)
#define assert_error(COND, MSG) assert_(COND, MSG, ERROR)
#define assert_fatal(COND, MSG) assert_(COND, MSG, FATAL)

#define assert_throw(COND, MSG) { if (!(COND)) throw std::runtime_error(MSG); }

#endif
