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

#include "video/font.hpp"

#include <algorithm>

#include "make_unique.hpp"

void
Font::flush_fonts()
{
  s_fonts.clear();
}

Font&
Font::get_font(const std::string& file, int size)
{
  auto it = std::find_if(s_fonts.begin(), s_fonts.end(),
                         [&file, size](const std::unique_ptr<Font>& font) {
    return font->m_name == file && font->m_size == size;
  });

  if (it == s_fonts.end())
  {
    s_fonts.push_back(std::make_unique<Font>(file, size));
    return **s_fonts.rbegin();
  }
  else
  {
    return **it;
  }
}

std::vector<std::unique_ptr<Font>> Font::s_fonts;

Font::Font(const std::string& text, int size) :
  m_name(text),
  m_size(size),
  m_font(TTF_OpenFont(text.c_str(), size))
{
}

Font::~Font()
{
  TTF_CloseFont(m_font);
}

SDL_Surface*
Font::get_sdl_surface(const std::string& text) const
{
  SDL_Color white;

  white.r = 255;
  white.g = 255;
  white.b = 255;
  white.a = 255;

  return TTF_RenderText_Blended(m_font, text.c_str(), white);
}

float
Font::get_text_width(const std::string& text) const
{
  SDL_Surface* s = get_sdl_surface(text);
  return static_cast<float>(s->w);
}

float
Font::get_text_height(const std::string& text) const
{
  SDL_Surface* s = get_sdl_surface(text);
  return static_cast<float>(s->h);
}
