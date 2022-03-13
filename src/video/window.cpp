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

#include "video/window.hpp"

#include "make_unique.hpp"

#if HARBOR_USE_VIDEO_SDL
#include "video/sdl/sdl_window.hpp"
#endif
#if HARBOR_USE_VIDEO_OPENGL
#include "video/gl/gl_window.hpp"
#endif

std::unique_ptr<Window>
Window::create_window(VideoSystem vs)
{
  switch(vs)
  {
#if HARBOR_USE_VIDEO_SDL
    case VideoSystem::SDL:
      return std::make_unique<SDLWindow>();
#endif

#if HARBOR_USE_VIDEO_OPENGL
    case VideoSystem::GL:
      return std::make_unique<GLWindow>();
#endif

    default:
      return nullptr;
  }
}

Window::Window() :
  m_texture_cache(),
  m_status_before_fullscreen(Status::NORMAL)
{
}

void
Window::flush_texture_cache()
{
  m_texture_cache.clear();
}

void
Window::toggle_fullscreen()
{
  if (get_status() == Status::FULLSCREEN)
  {
    set_status(m_status_before_fullscreen);
  }
  else
  {
    // set_status will automatically save the old status
    set_status(Status::FULLSCREEN);
  }
}
