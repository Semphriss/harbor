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
#include "SDL_surface.h"

#include <stdexcept>

#ifdef EMSCRIPTEN
#include "SDL2/SDL.h"
#else
#include "SDL.h"
#endif

#include "video/drawing_context.hpp"
#include "video/font.hpp"
#include "video/sdl/sdl_texture.hpp"
#include "video/sdl/sdl_window.hpp"
#include "util/color.hpp"
#include "util/rect.hpp"
#include "util/vector.hpp"

#if !SDL_VERSION_ATLEAST(2, 0, 10)
#define SDL_RenderFillRectF SDL_RenderFillRect
#define SDL_RenderCopyExF SDL_RenderCopyEx
#define SDL_RenderDrawLineF SDL_RenderDrawLine
#define SDL_FRect SDL_Rect
#endif

SDLRenderer::SDLRenderer(SDLWindow& window) :
  Renderer(window),
  m_sdl_renderer(SDL_CreateRenderer(window.get_sdl_window(), -1, 0))
{
  if (!m_sdl_renderer)
  {
    std::string error(SDL_GetError());
    throw std::runtime_error("Could not create SDL Renderer: " + error);
  }
}

SDLRenderer::~SDLRenderer()
{
  SDL_DestroyRenderer(m_sdl_renderer);
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

  if (&texture.get_window() != &get_window())
  {
    throw std::runtime_error("Attempt to use SDLRenderer::draw_texture() "
                             "with a texture not targeting the current window");
  }

  const SDLTexture& t = static_cast<const SDLTexture&>(texture);

  SDL_SetTextureColorMod(t.get_sdl_texture(),
                         static_cast<Uint8>(color.r * 255.f),
                         static_cast<Uint8>(color.g * 255.f),
                         static_cast<Uint8>(color.b * 255.f));
  SDL_SetTextureBlendMode(t.get_sdl_texture(),
                          static_cast<SDL_BlendMode>(blend));
  SDL_SetTextureAlphaMod(t.get_sdl_texture(),
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
  SDL_RenderCopyExF(m_sdl_renderer, t.get_sdl_texture(), &src, &dst, angle,
                    NULL, SDL_FLIP_NONE);
}

void
SDLRenderer::draw_text(const std::string& text, const Vector& pos,
                       const Rect& clip, TextAlign align,
                       const std::string& fontfile, int size,
                       const Color& color, const Blend& blend)
{
  if (!is_drawing())
  {
    throw std::runtime_error("Call to SDLRenderer::draw_text while not "
                             "drawing");
  }

  if (text.empty())
    return;

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
      dst.x -= surface->w / 2.f;
      break;

    case TextAlign::TOP_RIGHT:
      dst.x -= surface->w;
      break;

    case TextAlign::MID_LEFT:
      dst.y -= surface->h / 2.f;
      break;

    case TextAlign::CENTER:
      dst.x -= surface->w / 2.f;
      dst.y -= surface->h / 2.f;
      break;

    case TextAlign::MID_RIGHT:
      dst.x -= surface->w;
      dst.y -= surface->h / 2.f;
      break;

    case TextAlign::BOTTOM_LEFT:
      dst.y -= surface->h;
      break;

    case TextAlign::BOTTOM_MID:
      dst.x -= surface->w / 2.f;
      dst.y -= surface->h;
      break;

    case TextAlign::BOTTOM_RIGHT:
      dst.x -= surface->w;
      dst.y -= surface->h;
      break;
  }

  Rect srcrect(Vector(), Size(surface->w, surface->h));
  Rect dstrect(Vector(dst.x, dst.y), Size(dst.w, dst.h));
  srcrect = DrawingContext::clip_src_rect(srcrect, dstrect, clip);

  SDL_Rect src;
  src.x = static_cast<int>(srcrect.x1);
  src.y = static_cast<int>(srcrect.y1);
  src.w = static_cast<int>(srcrect.width());
  src.h = static_cast<int>(srcrect.height());

  dstrect.clip(clip);
  dst.x = dstrect.x1;
  dst.y = dstrect.y1;
  dst.w = dstrect.width();
  dst.h = dstrect.height();

  SDL_RenderCopy(m_sdl_renderer, texture, &src, &dst);
  SDL_FreeSurface(surface);
}

void
SDLRenderer::draw_line(const Vector& p1, const Vector& p2, const Color& color,
                       const Blend& blend)
{
  if (!is_drawing())
  {
    throw std::runtime_error("Call to SDLRenderer::draw_line while not "
                             "drawing");
  }

  SDL_SetRenderDrawColor(m_sdl_renderer,
                         static_cast<Uint8>(color.r * 255.f),
                         static_cast<Uint8>(color.g * 255.f),
                         static_cast<Uint8>(color.b * 255.f),
                         static_cast<Uint8>(color.a * 255.f));
  SDL_SetRenderDrawBlendMode(m_sdl_renderer, static_cast<SDL_BlendMode>(blend));

  SDL_RenderDrawLineF(m_sdl_renderer, p1.x, p1.y, p2.x, p2.y);
}

void
SDLRenderer::start_draw(Texture* texture)
{
  Renderer::start_draw();

  if (texture)
  {
    if (&texture->get_window() != &get_window())
    {
      throw std::runtime_error("Attempt to call SDLRenderer::start_draw() with "
                               "texture unrelated to current window");
    }

    const SDLTexture* sdl_texture = static_cast<SDLTexture*>(texture);
    SDL_SetRenderTarget(m_sdl_renderer, sdl_texture->get_sdl_texture());
  }
  else
  {
    SDL_SetRenderTarget(m_sdl_renderer, nullptr);
  }
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
