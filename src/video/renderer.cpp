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

#include "video/renderer.hpp"

#include <stdexcept>

#include "util/size.hpp"
#include "util/vector.hpp"
#include "video/font.hpp"

Rect
Renderer::get_text_rect(const std::string& font, int size, 
                        const std::string& text, const Vector& pos,
                        Renderer::TextAlign align)
{
  auto surface = get_font_surface(Font::get_font(font, size), text);
  Vector corner = pos;

  switch(align) {
    case TextAlign::TOP_LEFT:
      break;

    case TextAlign::TOP_MID:
      corner.x -= surface->w / 2;
      break;

    case TextAlign::TOP_RIGHT:
      corner.x -= surface->w;
      break;

    case TextAlign::MID_LEFT:
      corner.y -= surface->h / 2;
      break;

    case TextAlign::CENTER:
      corner.x -= surface->w / 2;
      corner.y -= surface->h / 2;
      break;

    case TextAlign::MID_RIGHT:
      corner.x -= surface->w;
      corner.y -= surface->h / 2;
      break;

    case TextAlign::BOTTOM_LEFT:
      corner.y -= surface->h;
      break;

    case TextAlign::BOTTOM_MID:
      corner.x -= surface->w / 2;
      corner.y -= surface->h;
      break;

    case TextAlign::BOTTOM_RIGHT:
      corner.x -= surface->w;
      corner.y -= surface->h;
      break;
  }

  return Rect(corner, Size(surface->w, surface->h));
}

SDL_Surface*
Renderer::get_font_surface(const Font& font, const std::string& text)
{
  return font.get_sdl_surface(text);
}

void
Renderer::start_draw(Texture* /* texture */)
{
  if (m_drawing)
  {
    throw std::runtime_error("Called Renderer::start_draw() on already drawing "
                             "renderer");
  }

  m_drawing = true;
}

void
Renderer::end_draw()
{
  if (!m_drawing)
  {
    throw std::runtime_error("Called Renderer::end_draw() on non-drawing "
                             "renderer");
  }

  m_drawing = false;
}

bool
Renderer::is_drawing() const
{
  return m_drawing;
}

Renderer::Renderer() :
  m_drawing(false)
{
}
