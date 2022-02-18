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

#include <stdexcept>

#include "make_unique.hpp"

#ifdef EMSCRIPTEN
#include "SDL2/SDL_image.h"
#else
#include "SDL_image.h"
#endif

#include "util/log.hpp"
#include "util/vector.hpp"
#include "video/sdl/sdl_texture.hpp"

SDLWindow::SDLWindow(const Size& size, bool visible) :
  m_sdl_window(SDL_CreateWindow("",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                static_cast<int>(size.w),
                                static_cast<int>(size.h),
                                // Intentionally ignores `visible` - see below
                                SDL_WINDOW_HIDDEN)),
  m_renderer(*this),
  m_icon_path()
{
  if (!m_sdl_window)
  {
    std::string error(SDL_GetError());
    throw std::runtime_error("Could not create SDL Window: " + error);
  }

  // If `SDL_WINDOW_SHOWN` is used in SDL_CreateWindow, the window flickers a
  // bit upon creation. Calling SDL_ShowWindow fixes the problem.
  if (visible)
  {
    SDL_ShowWindow(m_sdl_window);
  }
}

SDLWindow::~SDLWindow()
{
  SDL_DestroyWindow(m_sdl_window);
}

Window::VideoSystem
SDLWindow::get_type() const
{
  return VideoSystem::SDL;
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

std::shared_ptr<Texture>
SDLWindow::create_texture(const Size& size)
{
  return std::make_shared<SDLTexture>(*this, size);
}

Renderer&
SDLWindow::get_renderer()
{
  return m_renderer;
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

bool
SDLWindow::get_visible() const
{
  return static_cast<bool>(SDL_GetWindowFlags(m_sdl_window) & SDL_WINDOW_SHOWN);
}

Vector
SDLWindow::get_pos() const
{
  int x, y;
  SDL_GetWindowPosition(m_sdl_window, &x, &y);
  return Vector(static_cast<float>(x), static_cast<float>(y));
}

bool
SDLWindow::get_bordered() const
{
  return !static_cast<bool>(SDL_GetWindowFlags(m_sdl_window)
                            & SDL_WINDOW_BORDERLESS);
}

bool
SDLWindow::get_resizable() const
{
  return static_cast<bool>(SDL_GetWindowFlags(m_sdl_window)
                            & SDL_WINDOW_RESIZABLE);
}

Window::Status
SDLWindow::get_status() const
{
  const auto& flags = SDL_GetWindowFlags(m_sdl_window);

  if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
    return Status::FULLSCREEN;

  if (flags & SDL_WINDOW_MAXIMIZED)
    return Status::MAXIMIZED;

  if (flags & SDL_WINDOW_MINIMIZED)
    return Status::MINIMIZED;

  return Status::NORMAL;
}

float
SDLWindow::get_opacity() const
{
  float opacity;
  if (SDL_GetWindowOpacity(m_sdl_window, &opacity))
  {
    std::string error(SDL_GetError());
    throw std::runtime_error("Could not get window opacity: " + error);
  }
  return opacity;
}

std::string
SDLWindow::get_icon() const
{
  return m_icon_path;
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

void
SDLWindow::set_visible(bool visible)
{
  if (visible)
  {
    SDL_ShowWindow(m_sdl_window);
  }
  else
  {
    SDL_HideWindow(m_sdl_window);
  }
}

void
SDLWindow::set_pos(Vector pos)
{
  int x = static_cast<int>(pos.x);
  int y = static_cast<int>(pos.y);

  SDL_SetWindowPosition(m_sdl_window, x, y);
}

void
SDLWindow::set_bordered(bool bordered)
{
  SDL_SetWindowBordered(m_sdl_window, bordered ? SDL_TRUE : SDL_FALSE);
}

void
SDLWindow::set_resizable(bool resizable)
{
#if SDL_VERSION_ATLEAST(2, 0, 5)
  SDL_SetWindowResizable(m_sdl_window, resizable ? SDL_TRUE : SDL_FALSE);
#else
  log_info << "Cannot set SDLWindow resizable because it requires SDL >= 2.0.5"
           << " and the compiled version is " << SDL_MAJOR_VERSION << "."
           << SDL_MINOR_VERSION << "." << SDL_PATCHLEVEL << std::endl;
#endif
}

void
SDLWindow::set_status(Status status)
{
  switch (status)
  {
    case Status::NORMAL:
      if (SDL_SetWindowFullscreen(m_sdl_window, 0))
      {
        std::string error(SDL_GetError());
        throw std::runtime_error("Could not restore window: " + error);
      }
      SDL_RestoreWindow(m_sdl_window);
      break;

    case Status::MINIMIZED:
      if (SDL_SetWindowFullscreen(m_sdl_window, 0))
      {
        std::string error(SDL_GetError());
        throw std::runtime_error("Could not restore window: " + error);
      }
      SDL_MinimizeWindow(m_sdl_window);
      break;

    case Status::MAXIMIZED:
      if (SDL_SetWindowFullscreen(m_sdl_window, 0))
      {
        std::string error(SDL_GetError());
        throw std::runtime_error("Could not restore window: " + error);
      }
      SDL_MaximizeWindow(m_sdl_window);
      break;

    case Status::FULLSCREEN:
      if (SDL_SetWindowFullscreen(m_sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP))
      {
        std::string error(SDL_GetError());
        throw std::runtime_error("Could not make window fullscreen: " + error);
      }
      break;
  }
}

void
SDLWindow::set_icon(const std::string& filename)
{
  m_icon_path = filename;

  SDL_Surface* surface = IMG_Load(filename.c_str());

  if (!surface)
  {
    std::string error(IMG_GetError());
    throw std::runtime_error("Could not load window icon: " + error);
  }

  SDL_SetWindowIcon(m_sdl_window, surface);
}

void
SDLWindow::set_opacity(float opacity)
{
  if (SDL_SetWindowOpacity(m_sdl_window, opacity))
  {
    std::string error(SDL_GetError());
    throw std::runtime_error("Could not set window opacity " + error);
  }
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
