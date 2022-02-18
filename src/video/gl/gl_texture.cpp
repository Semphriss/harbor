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

#include "video/gl/gl_texture.hpp"

#include <stdexcept>
#include <string>

#ifdef EMSCRIPTEN
#include "SDL2/SDL_image.h"
#else
#include "SDL_image.h"
#endif

#include "video/gl/gl_window.hpp"

GLTexture::GLTexture(GLWindow& window, const Size& size) :
  Texture(window, size),
  m_renderer(window.get_glrenderer()),
  m_gl_texture(),
  m_sdl_surface(nullptr)
{
  glGenTextures(1, &m_gl_texture);
  glBindTexture(GL_TEXTURE_2D, m_gl_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int>(size.w),
               static_cast<int>(size.h), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

GLTexture::GLTexture(GLWindow& window, const std::string& file) :
  Texture(window, Size()),
  m_renderer(window.get_glrenderer()),
  m_gl_texture(),
  m_sdl_surface(nullptr)
{
  SDL_Surface* image = IMG_Load(file.c_str());

  if (!image)
  {
    throw std::runtime_error("GLTexture could not load image: " + file);
  }

  m_sdl_surface = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ABGR8888, 0);
  SDL_FreeSurface(image);

  m_size.w = static_cast<float>(m_sdl_surface->w);
  m_size.h = static_cast<float>(m_sdl_surface->h);

  glGenTextures(1, &m_gl_texture);
  glBindTexture(GL_TEXTURE_2D, m_gl_texture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_sdl_surface->w, m_sdl_surface->h, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, m_sdl_surface->pixels);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

GLTexture::~GLTexture()
{
  glDeleteTextures(1, &m_gl_texture);

  if (m_sdl_surface)
  {
    SDL_FreeSurface(m_sdl_surface);
  }
}

GLuint
GLTexture::get_gl_texture() const
{
  return m_gl_texture;
}
