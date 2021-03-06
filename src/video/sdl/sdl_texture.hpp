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

#ifndef _HEADER_HARBOR_VIDEO_SDL_SDLTEXTURE_HPP
#define _HEADER_HARBOR_VIDEO_SDL_SDLTEXTURE_HPP

#include "video/texture.hpp"

#include <memory>

#include "video/sdl/sdl_renderer.hpp"

struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;

/**
 * Class that represents a readable texture.
 */
class SDLTexture final :
  public Texture
{
public:
  SDLTexture(SDLWindow& window, const Size& size);
  SDLTexture(SDLWindow& window, const std::string& file);
  virtual ~SDLTexture() override;

  SDL_Texture* get_sdl_texture() const;

private:
  SDLRenderer& m_renderer;
  SDL_Texture* m_sdl_texture;

private:
  SDLTexture(const SDLTexture&) = delete;
  SDLTexture& operator=(const SDLTexture&) = delete;
};

#endif
