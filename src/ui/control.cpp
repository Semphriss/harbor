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

#include "ui/control.hpp"

#include "SDL.h"

#include "ui/container.hpp"

Control::Control(int layer, const Rect& rect, const ThemeSet& theme,
                 Container* parent) :
  m_rect(rect),
  m_theme(theme),
  m_parent(parent),
  m_focused(false),
  m_disabled(false),
  m_layer(layer)
{
}

Control::~Control()
{
}

Control::ThemeSet&
Control::get_theme()
{
  return m_theme;
}

Rect&
Control::get_rect()
{
  return m_rect;
}

const Control::ThemeSet&
Control::get_theme() const
{
  return m_theme;
}

const Rect&
Control::get_rect() const
{
  return m_rect;
}

bool
Control::is_disabled() const
{
  return m_disabled;
}

void
Control::set_disabled(bool disabled)
{
  m_disabled = disabled;
}

Container*
Control::get_parent() const
{
  return m_parent;
}

void
Control::set_focused(bool focused, bool call_parent)
{
  m_focused = focused;

  if (call_parent && m_parent)
  {
    m_parent->set_focus(*this, focused);
  }
}

bool
Control::is_focused() const
{
  return m_focused;
}
