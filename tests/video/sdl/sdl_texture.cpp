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

#include "SDL.h"

#include "util/size.hpp"
#include "video/sdl/sdl_window.hpp"

#define private public
#define protected public
#define final

#include "video/sdl/sdl_texture.hpp"

#undef private
#undef protected
#undef final
/*
TEST(Video_SDL_SDLTexture, from_file)
{
  SDLWindow w;
  SDLTexture t(w, "../data/images/missing.png");

  int width, height;
  SDL_QueryTexture(t.get_sdl_texture(), nullptr, nullptr, &width, &height);

  ASSERT_EQ(width, t.get_size().w);
  ASSERT_EQ(height, t.get_size().h);

  ASSERT_EQ(t.get_size().w, 128);
  ASSERT_EQ(t.get_size().h, 128);
}

TEST(Video_SDL_SDLTexture, canvas)
{
  SDLWindow w;
  SDLTexture t(w, Size(137, 141));

  int width, height;
  SDL_QueryTexture(t.get_sdl_texture(), nullptr, nullptr, &width, &height);

  ASSERT_EQ(width, t.get_size().w);
  ASSERT_EQ(height, t.get_size().h);

  ASSERT_EQ(t.get_size().w, 137);
  ASSERT_EQ(t.get_size().h, 141);
}
*/
