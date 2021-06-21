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

#include "SDL.h"

#include "video/font.hpp"
#include "video/sdl/sdl_texture.hpp"
#include "video/sdl/sdl_window.hpp"
#include "util/color.hpp"
#include "util/rect.hpp"
#include "util/vector.hpp"

SDLRenderer::SDLRenderer(SDLWindow& window) :
  Renderer(window),
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
SDLRenderer::draw_texture(const Texture& texture, const Rect& srcrect,
                          const Rect& dstrect, float angle, const Color& color,
                          const Blend& blend)
{
  if (!is_drawing())
  {
    throw std::runtime_error("Call to SDLRenderer::draw_texture while not "
                             "drawing");
  }

  const SDLTexture* t = dynamic_cast<const SDLTexture*>(&texture);

  if (!t)
  {
    throw std::runtime_error("Attempt to use SDLRenderer::draw_texture() "
                             "with a non-SDL texture");
  }

  SDL_SetTextureColorMod(t->get_sdl_texture(),
                         static_cast<Uint8>(color.r * 255.f),
                         static_cast<Uint8>(color.g * 255.f),
                         static_cast<Uint8>(color.b * 255.f));
  SDL_SetTextureBlendMode(t->get_sdl_texture(),
                          static_cast<SDL_BlendMode>(blend));
  SDL_SetTextureAlphaMod(t->get_sdl_texture(),
                         static_cast<Uint8>(color.a * 255.f));



  SDL_Rect src;
  SDL_FRect dst;
  src.x = static_cast<int>(srcrect.x1);
  src.y = static_cast<int>(srcrect.y1);
  src.w = static_cast<int>(srcrect.width());
  src.h = static_cast<int>(srcrect.height());
  dst.x = dstrect.x1;
  dst.y = dstrect.y1;
  dst.w = dstrect.width();
  dst.h = dstrect.height();

  // TODO: Add support for center point and flip
  SDL_RenderCopyExF(m_sdl_renderer, t->get_sdl_texture(), &src, &dst, angle,
                    NULL, SDL_FLIP_NONE);
}

void
SDLRenderer::draw_text(const std::string& text, const Vector& pos,
                       TextAlign align, const std::string& fontfile, int size,
                       const Color& color, const Blend& blend)
{
  if (!is_drawing())
  {
    throw std::runtime_error("Call to SDLRenderer::draw_text while not "
                             "drawing");
  }

  auto& font = Font::get_font(fontfile, size);

  SDL_Surface* surface = get_font_surface(font, text);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(m_sdl_renderer, surface);

  SDL_SetTextureColorMod(texture,
                         static_cast<Uint8>(color.r * 255.f),
                         static_cast<Uint8>(color.g * 255.f),
                         static_cast<Uint8>(color.b * 255.f));
  SDL_SetTextureBlendMode(texture, static_cast<SDL_BlendMode>(blend));
  SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(color.a * 255.f));

  SDL_Rect dst;
  dst.x = static_cast<int>(pos.x);
  dst.y = static_cast<int>(pos.y);
  dst.w = surface->w;
  dst.h = surface->h;

  switch(align) {
    case TextAlign::TOP_LEFT:
      break;

    case TextAlign::TOP_MID:
      dst.x -= surface->w / 2;
      break;

    case TextAlign::TOP_RIGHT:
      dst.x -= surface->w;
      break;

    case TextAlign::MID_LEFT:
      dst.y -= surface->h / 2;
      break;

    case TextAlign::CENTER:
      dst.x -= surface->w / 2;
      dst.y -= surface->h / 2;
      break;

    case TextAlign::MID_RIGHT:
      dst.x -= surface->w;
      dst.y -= surface->h / 2;
      break;

    case TextAlign::BOTTOM_LEFT:
      dst.y -= surface->h;
      break;

    case TextAlign::BOTTOM_MID:
      dst.x -= surface->w / 2;
      dst.y -= surface->h;
      break;

    case TextAlign::BOTTOM_RIGHT:
      dst.x -= surface->w;
      dst.y -= surface->h;
      break;
  }

  SDL_RenderCopy(m_sdl_renderer, texture, nullptr, &dst);
}

void
SDLRenderer::start_draw(Texture* texture)
{
  Renderer::start_draw();

  auto* sdl_texture = dynamic_cast<SDLTexture*>(texture);

  if (texture && !sdl_texture)
  {
    throw std::runtime_error("Attempt to call SDLRenderer::start_draw() with "
                             "non-null but non-SDL texture");
  }

  SDL_SetRenderTarget(m_sdl_renderer, texture
                                      ? sdl_texture->get_sdl_texture()
                                      : nullptr);
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

SDL_Renderer*
SDLRenderer::get_sdl_renderer() const
{
  return m_sdl_renderer;
}
