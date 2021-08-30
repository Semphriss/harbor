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

#include "util/color.hpp"
#include "util/rect.hpp"
#include "video/sdl/sdl_window.hpp"

#define private public
#define protected public
#define final

#include "video/sdl/sdl_renderer.hpp"

#undef private
#undef protected
#undef final

TEST(Video_SDL_SDLRenderer, is_drawing)
{
  SDLWindow w;
  auto& r = w.get_renderer();

  ASSERT_EQ(r.is_drawing(), false);
  ASSERT_THROW(r.end_draw(), std::runtime_error);
  r.start_draw();
  ASSERT_EQ(r.is_drawing(), true);
  ASSERT_THROW(r.start_draw(), std::runtime_error);
  r.end_draw();
  ASSERT_EQ(r.is_drawing(), false);
  ASSERT_THROW(r.end_draw(), std::runtime_error);
}

TEST(Video_SDL_SDLRenderer, draw_filled_rect)
{
  SDLWindow w;
  auto& r = w.get_renderer();

  ASSERT_THROW(r.draw_filled_rect({0, 0, 0, 0}, {0, 0, 0}, Renderer::Blend::NONE), std::runtime_error);
  r.start_draw();
  ASSERT_NO_THROW(r.draw_filled_rect({0, 0, 0, 0}, {0, 0, 0}, Renderer::Blend::NONE));
  r.end_draw();
  ASSERT_THROW(r.draw_filled_rect({0, 0, 0, 0}, {0, 0, 0}, Renderer::Blend::NONE), std::runtime_error);

  // TODO: Eventually, check to see if drawing really occured properly
}
