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

#include "video/renderer.hpp"

#include <stdexcept>

void
Renderer::start_draw()
{
  if (m_drawing)
  {
    throw std::runtime_error("Called Renderer::start_draw() on already drawing "
                             "renderer");
  }

  m_drawing = true;
}

void
Renderer::end_draw()
{
  if (!m_drawing)
  {
    throw std::runtime_error("Called Renderer::end_draw() on non-drawing "
                             "renderer");
  }

  m_drawing = false;
}

bool
Renderer::is_drawing() const
{
  return m_drawing;
}

Renderer::Renderer() :
  m_drawing(false)
{
}
