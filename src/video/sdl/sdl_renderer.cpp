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

#include "video/sdl/sdl_renderer.hpp"

#include <stdexcept>

#include <SDL.h>

#include "video/sdl/sdl_window.hpp"
#include "util/color.hpp"
#include "util/rect.hpp"

SDLRenderer::SDLRenderer(SDLWindow& window) :
  m_sdl_renderer(SDL_CreateRenderer(window.get_sdl_window(), -1, 0))
{

}

void
SDLRenderer::draw_filled_rect(const Rect& rect, const Color& color,
                              const Blend& blend)
{
  if (!is_drawing())
  {
    throw std::runtime_error("Call to SDLRenderer::draw_filled_rect while not "
                             "drawing");
  }

  SDL_SetRenderDrawColor(m_sdl_renderer,
                         static_cast<Uint8>(color.r * 255.f),
                         static_cast<Uint8>(color.g * 255.f),
                         static_cast<Uint8>(color.b * 255.f),
                         static_cast<Uint8>(color.a * 255.f));
  SDL_SetRenderDrawBlendMode(m_sdl_renderer, static_cast<SDL_BlendMode>(blend));

  SDL_FRect sdl_rect;
  sdl_rect.x = rect.x1;
  sdl_rect.y = rect.y1;
  sdl_rect.w = rect.width();
  sdl_rect.h = rect.height();
  SDL_RenderFillRectF(m_sdl_renderer, &sdl_rect);
}

void
SDLRenderer::start_draw()
{
  Renderer::start_draw();
  SDL_SetRenderTarget(m_sdl_renderer, nullptr);
}

void
SDLRenderer::end_draw()
{
  Renderer::end_draw();

  if (!SDL_GetRenderTarget(m_sdl_renderer))
  {
    SDL_RenderPresent(m_sdl_renderer);
  }
  else
  {
    SDL_SetRenderTarget(m_sdl_renderer, nullptr);
  }

  SDL_SetRenderDrawColor(m_sdl_renderer, 0, 0, 0, 0);
  SDL_RenderClear(m_sdl_renderer);
}
