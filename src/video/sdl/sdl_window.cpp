//  Harbor - A portable and highly customisable game engine
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
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

SDLWindow::SDLWindow() :
  m_window()
{
  m_window = SDL_CreateWindow("",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              640,
                              400,
                              SDL_WINDOW_SHOWN);
}

SDLWindow::~SDLWindow()
{
  SDL_DestroyWindow(m_window);
}

void
SDLWindow::set_title(const std::string& title)
{
  SDL_SetWindowTitle(m_window, title.c_str());
}

std::string
SDLWindow::get_title() const
{
  return std::string(SDL_GetWindowTitle(m_window));
}
