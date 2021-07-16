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

#include "video/sdl/sdl_texture.hpp"

#include "video/sdl/sdl_window.hpp"

#include <stdexcept>
#include <string>

#include "SDL.h"
#include "SDL_image.h"

SDLTexture::SDLTexture(SDLWindow& window, const Size& size) :
  Texture(size),
  m_renderer(window.get_sdlrenderer()),
  m_sdl_texture(SDL_CreateTexture(m_renderer.get_sdl_renderer(),
                                  SDL_PIXELFORMAT_RGBA8888,
                                  SDL_TEXTUREACCESS_TARGET,
                                  static_cast<int>(size.w),
                                  static_cast<int>(size.h)))
{
  if (!m_sdl_texture)
  {
    throw std::runtime_error("Could not create SDL Texture: " +
                              std::string(SDL_GetError()));
  }
}

SDLTexture::SDLTexture(SDLWindow& window, const std::string& file) :
  Texture(Size()),
  m_renderer(window.get_sdlrenderer()),
  m_sdl_texture(IMG_LoadTexture(m_renderer.get_sdl_renderer(), file.c_str()))
{
  if (!m_sdl_texture)
  {
    throw std::runtime_error("Could not load SDL Texture: " +
                              std::string(SDL_GetError()));
  }

  int w, h;
  SDL_QueryTexture(m_sdl_texture, nullptr, nullptr, &w, &h);
  m_size.w = static_cast<float>(w);
  m_size.h = static_cast<float>(h);
}

SDLTexture::~SDLTexture()
{
  if (m_sdl_texture)
  {
    SDL_DestroyTexture(m_sdl_texture);
  }
}

SDL_Texture*
SDLTexture::get_sdl_texture() const
{
  return m_sdl_texture;
}
