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

#include "gtest/gtest.h"

#include "video/sdl/sdl_window.hpp"

TEST(Video_SDL_SDLWindow, set_title)
{
  SDLWindow w;

  w.set_title("Lorem ipsum dolor");
  ASSERT_EQ(w.get_title(), "Lorem ipsum dolor");

  w.set_title("Caractères spéciaux!");
  ASSERT_EQ(w.get_title(), "Caractères spéciaux!");

  w.set_title("こんにちは世界！ 🙂");
  ASSERT_EQ(w.get_title(), "こんにちは世界！ 🙂");
}
