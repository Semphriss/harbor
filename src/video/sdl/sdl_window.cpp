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

#include "video/sdl/sdl_window.hpp"

#include "make_unique.hpp"

#include "video/sdl/sdl_texture.hpp"

SDLWindow::SDLWindow() :
  m_sdl_window(SDL_CreateWindow("",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                640,
                                400,
                                SDL_WINDOW_HIDDEN)),
  m_renderer(*this)
{
  SDL_ShowWindow(m_sdl_window);
}

SDLWindow::~SDLWindow()
{
  SDL_DestroyWindow(m_sdl_window);
}

Texture&
SDLWindow::load_texture(const std::string& file)
{
  const auto& texture_ptr = m_texture_cache[std::string(file)];

  if (!texture_ptr)
  {
    auto new_texture = std::make_unique<SDLTexture>(*this, file);
    m_texture_cache[std::string(file)] = std::move(new_texture);
    return *m_texture_cache[std::string(file)];
  }

  return *texture_ptr;
}

std::unique_ptr<Texture>
SDLWindow::create_texture(const Size& size)
{
  return std::make_unique<SDLTexture>(*this, size);
}

std::string
SDLWindow::get_title() const
{
  return std::string(SDL_GetWindowTitle(m_sdl_window));
}

Size
SDLWindow::get_size() const
{
  int w, h;
  SDL_GetWindowSize(m_sdl_window, &w, &h);
  return Size(static_cast<float>(w), static_cast<float>(h));
}

void
SDLWindow::set_size(const Size& size)
{
  SDL_SetWindowSize(m_sdl_window, static_cast<int>(size.w),
                    static_cast<int>(size.h));
}

void
SDLWindow::set_title(const std::string& title)
{
  SDL_SetWindowTitle(m_sdl_window, title.c_str());
}

Renderer&
SDLWindow::get_renderer()
{
  return m_renderer;
}

SDL_Window*
SDLWindow::get_sdl_window() const
{
  return m_sdl_window;
}

SDLRenderer&
SDLWindow::get_sdlrenderer()
{
  return m_renderer;
}
