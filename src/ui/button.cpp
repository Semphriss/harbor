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

#include "ui/button.hpp"

#include "SDL_events.h"

#include "ui/container.hpp"
#include "video/drawing_context.hpp"

Button::Button(std::function<void(int)> on_click, int btnmask, bool onbtnup,
               int layer, const Rect& rect, const ThemeSet& theme,
               Container* parent) :
  Control(layer, rect, theme, parent),
  m_on_click(on_click),
  m_mouse_pos(),
  m_mouse_button_pressed(0),
  m_btnmask(btnmask),
  m_onbtnup(onbtnup)
{
}

bool
Button::event(const SDL_Event& event)
{
  switch (event.type)
  {
    case SDL_MOUSEBUTTONUP:
    {
      int mask = 0;
      switch (event.button.button)
      {
        case SDL_BUTTON_LEFT:
          mask = 0x01;
          break;

        case SDL_BUTTON_RIGHT:
          mask = 0x02;
          break;

        case SDL_BUTTON_MIDDLE:
          mask = 0x04;
          break;

        case SDL_BUTTON_X1:
          mask = 0x08;
          break;

        case SDL_BUTTON_X2:
          mask = 0x10;
          break;
      }

      m_mouse_button_pressed &= ~mask;

      bool on_this = m_rect.contains(Vector(event.button.x, event.button.y));
      if (on_this)
      {
        set_focused(true, true);
        if (m_onbtnup && (m_btnmask & mask))
        {
          m_on_click(mask);
        }
      }

      return on_this;
    }

    case SDL_MOUSEBUTTONDOWN:
    {
      int mask = 0;
      switch (event.button.button)
      {
        case SDL_BUTTON_LEFT:
          mask = 0x01;
          break;

        case SDL_BUTTON_RIGHT:
          mask = 0x02;
          break;

        case SDL_BUTTON_MIDDLE:
          mask = 0x04;
          break;

        case SDL_BUTTON_X1:
          mask = 0x08;
          break;

        case SDL_BUTTON_X2:
          mask = 0x10;
          break;
      }

      m_mouse_button_pressed |= mask;

      bool on_this = m_rect.contains(Vector(event.button.x, event.button.y));
      if (on_this)
      {
        set_focused(true, true);
        if (!m_onbtnup && (m_btnmask & mask))
        {
          m_on_click(mask);
        }
      }

      return on_this;
    }

    case SDL_KEYDOWN:
      if (is_focused() && !m_onbtnup)
      {
        switch (event.key.keysym.sym)
        {
          case SDLK_SPACE:
          case SDLK_RETURN:
          case SDLK_RETURN2:
            m_on_click(0);
            break;
          
          default:
            break;
        }
      }
      return is_focused();

    case SDL_KEYUP:
      if (is_focused() && m_onbtnup)
      {
        switch (event.key.keysym.sym)
        {
          case SDLK_SPACE:
          case SDLK_RETURN:
          case SDLK_RETURN2:
            m_on_click(0);
            break;
          
          default:
            break;
        }
      }
      return is_focused();

    case SDL_MOUSEMOTION:
      m_mouse_pos = Vector(event.motion.x, event.motion.y);
      return false;

    default:
      break;
  }

  return false;
}

void
Button::update(float /* dt_sec */)
{
}

void
Button::draw(DrawingContext& context) const
{
  const auto& theme = get_current_theme();
  context.draw_filled_rect(m_rect.grown(1).moved({0,0}), Color(0,0,0,0.1*theme.bg_color.a), Renderer::Blend::BLEND, m_layer);
  context.draw_filled_rect(m_rect.grown(1).moved({0,-1}), Color(0,0,0,0.1*theme.bg_color.a), Renderer::Blend::BLEND, m_layer);
  context.draw_filled_rect(m_rect.grown(1).moved({0,-2}), Color(0,0,0,0.1*theme.bg_color.a), Renderer::Blend::BLEND, m_layer);
  context.draw_filled_rect(m_rect.grown(1).moved({0,-3}), Color(0,0,0,0.1*theme.bg_color.a), Renderer::Blend::BLEND, m_layer);
  context.draw_filled_rect(m_rect, theme.bg_color, Renderer::Blend::BLEND, m_layer);
}

const Control::Theme&
Button::get_current_theme() const
{
  if (m_disabled)
    return m_theme.disabled;

  if (m_rect.contains(m_mouse_pos))
  {
    if (m_mouse_button_pressed & m_btnmask)
    {
      return m_theme.active;
    }
    else
    {
      return m_theme.hover;
    }
  }
  else
  {
    if (m_parent && m_parent->get_focused_child() == this)
    {
      return m_theme.focus;
    }
    else
    {
      return m_theme.normal;
    }
  }
}
