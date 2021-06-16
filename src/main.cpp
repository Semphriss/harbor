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

#include "SDL.h"

#include "util/color.hpp"
#include "util/rect.hpp"
#include "video/drawing_context.hpp"
#include "video/sdl/sdl_window.hpp"

int main()
{
  auto w = Window::create_window(Window::VideoSystem::SDL);
  w->set_title("Hello, world!");

  SDL_Event e;
  bool quit = false;
  while(!quit)
  {
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
      switch(e.type)
      {
        case SDL_QUIT:
          quit = true;
          break;

        default:
          break;
      }
    }

    auto& r = w->get_renderer();
    DrawingContext dc(r);
    dc.draw_filled_rect(Rect(20, 10, 100, 300), Color(0.5f, 0.25f, 0.125f),
                        Renderer::Blend::BLEND, 5);
    dc.draw_filled_rect(Rect(10, 5, 400, 50), Color(0.5f, 0.25f, 1.f),
                        Renderer::Blend::BLEND, 1);
    dc.render();
  }

  return 0;
}
