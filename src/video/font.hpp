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

#ifndef _HEADER_HARBOR_VIDEO_FONT_HPP
#define _HEADER_HARBOR_VIDEO_FONT_HPP

#include <string>
#include <memory>
#include <vector>

#include "make_unique.hpp"

#include "SDL_ttf.h"

/**
 * Video class to handle font files.
 */
class Font final
{
  friend class Renderer;

public:
  static void flush_fonts();
  static Font& get_font(const std::string& file, int size);

private:
  static std::vector<std::unique_ptr<Font>> s_fonts;

public:
  Font(const std::string& text, int size);
  ~Font();

private:
  SDL_Surface* get_sdl_surface(const std::string& text) const;

public:
  float get_text_width(const std::string& text) const;
  float get_text_height(const std::string& text) const;

private:
  std::string m_name;
  int m_size;
  TTF_Font* m_font;

private:
  Font(const Font&) = delete;
  Font& operator=(const Font&) = delete;
};

#endif
