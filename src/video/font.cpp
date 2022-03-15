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
#include <memory>
#include <stdexcept>

#include "SDL_surface.h"
#include "SDL_ttf.h"
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
  m_font(TTF_OpenFont(text.c_str(), size)),
  m_cache()
{
  if (!m_font)
  {
    throw std::runtime_error("Could not open font '" + text + "': " +
                             std::string(TTF_GetError()));
  }
}

Font::~Font()
{
  TTF_CloseFont(m_font);
}

SDL_Surface*
Font::get_sdl_surface(const std::string& text)
{
  try
  {
    return m_cache.at(text).get();
  }
  catch(const std::out_of_range&)
  {
    SDL_Color white;

    white.r = 255;
    white.g = 255;
    white.b = 255;
#ifndef EMSCRIPTEN
    white.a = 255;
#endif

    auto* s = TTF_RenderText_Blended_Wrapped(m_font, text.c_str(), white, 9999);
    m_cache.emplace(std::pair<std::string, TextSurface>{text, TextSurface(s, SDL_FreeSurface)});

    return s;
  }
}

float
Font::get_text_width(const std::string& text) const
{
  return get_text_size(text).w;
}

float
Font::get_text_height(const std::string& text) const
{
  return get_text_size(text).h;
}

Size
Font::get_text_size(const std::string& text) const
{
  int w, h;

  if (TTF_SizeText(m_font, text.c_str(), &w, &h))
  {
    throw std::runtime_error("Could not get text dimensions: "
                             + std::string(TTF_GetError()));
  }

  return Size(static_cast<float>(w), static_cast<float>(h));
}
