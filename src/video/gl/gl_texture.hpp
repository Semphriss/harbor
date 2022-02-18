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

#ifndef _HEADER_HARBOR_VIDEO_GL_GLTEXTURE_HPP
#define _HEADER_HARBOR_VIDEO_GL_GLTEXTURE_HPP

#include "video/texture.hpp"

#include <memory>

#ifdef EMSCRIPTEN
#include "SDL2/SDL_opengl.h"
#else
#include "SDL_opengl.h"
#endif

#include "video/gl/gl_renderer.hpp"

/**
 * Class that represents a readable texture.
 */
class GLTexture final :
  public Texture
{
public:
  GLTexture(GLWindow& window, const Size& size);
  GLTexture(GLWindow& window, const std::string& file);
  virtual ~GLTexture() override;

  GLuint get_gl_texture() const;

private:
  GLRenderer& m_renderer;
  GLuint m_gl_texture;
  SDL_Surface* m_sdl_surface;

private:
  GLTexture(const GLTexture&) = delete;
  GLTexture& operator=(const GLTexture&) = delete;
};

#endif
