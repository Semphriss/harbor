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

#include "ui/scrollbar.hpp"

#include "ui/container.hpp"
#include "util/math.hpp"
#include "video/drawing_context.hpp"

Scrollbar::Scrollbar(std::function<void(float)> on_scroll, float cover,
                     float total, bool horizontal, int btnmask, int layer,
                     const Rect& rect, const ThemeSet& theme,
                     Container* parent) :
  Control(layer, rect, theme, parent),
  m_on_scroll(on_scroll),
  m_mouse_pos(),
  m_mouse_button_pressed(),
  m_btnmask(btnmask),
  m_progress(0.f),
  m_cover(cover),
  m_total(total),
  m_horizontal(horizontal),
  m_dragging(false)
{
}

bool
Scrollbar::event(const SDL_Event& event)
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

      bool was_dragging = m_dragging;

      m_mouse_button_pressed &= ~mask;
      m_dragging = m_mouse_button_pressed & m_btnmask;

      return was_dragging;
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

      if (m_rect.contains(Vector(event.button.x, event.button.y))
          && (m_mouse_button_pressed & m_btnmask))
      {
        set_focused(true, true);
        m_dragging = true;
        return true;
      }

      break;
    }

    case SDL_KEYDOWN:
      if (is_focused())
      {
        switch (event.key.keysym.sym)
        {
          case SDLK_UP:
          case SDLK_LEFT:
            if (m_progress > 0)
            {
              m_progress--;
              if (m_on_scroll)
                m_on_scroll(m_progress);
            }
            return true;

          case SDLK_DOWN:
          case SDLK_RIGHT:
            if (m_progress < m_total - m_cover)
            {
              m_progress++;
              if (m_on_scroll)
                m_on_scroll(m_progress);
            }
            return true;

          case SDLK_PAGEUP:
          case SDLK_HOME:
            if (m_progress != 0)
            {
              m_progress = 0;
              if (m_on_scroll)
                m_on_scroll(m_progress);
            }
            return true;

          case SDLK_PAGEDOWN:
          case SDLK_END:
            if (m_progress != m_total - m_cover)
            {
              m_progress = m_total - m_cover;
              if (m_on_scroll)
                m_on_scroll(m_progress);
            }
            return true;

          default:
            break;
        }
      }
      break;

    case SDL_MOUSEMOTION:
      m_mouse_pos = Vector(event.motion.x, event.motion.y);
      if (m_dragging)
      {
        m_progress += static_cast<float>(m_horizontal ? event.motion.xrel :
                      event.motion.yrel) / static_cast<float>(m_horizontal ?
                      m_rect.width() : m_rect.height()) * m_total;

        adjust_bar();

        if (m_on_scroll)
          m_on_scroll(m_progress);

        return true;
      }
      break;

    case SDL_MOUSEWHEEL:
      if (m_rect.contains(m_mouse_pos))
      {
        int delta = (m_horizontal ? event.wheel.x : event.wheel.y);
        set_progress(get_progress() - delta * 10);
        return true;
      }
      return false;

    default:
      break;
  }

  return false;
}

void
Scrollbar::update(float /* dt_sec */)
{
}

void
Scrollbar::draw(DrawingContext& context) const
{
  const auto& theme = get_current_theme();
  context.draw_filled_rect(m_rect, theme.bg_color, Renderer::Blend::BLEND, m_layer);
  Rect bar = m_rect;
  (m_horizontal ? bar.x1 : bar.y1) += m_progress / m_total * (m_horizontal ? m_rect.width() : m_rect.height());
  (m_horizontal ? bar.x2 : bar.y2) = (m_horizontal ? bar.x1 : bar.y1) + m_cover / m_total * (m_horizontal ? m_rect.width() : m_rect.height());
  context.draw_filled_rect(bar, theme.fg_color, Renderer::Blend::BLEND, m_layer);
}

void
Scrollbar::set_progress(float progress)
{
  m_progress = progress;
  adjust_bar();
}

void
Scrollbar::set_cover(float cover)
{
  m_cover = cover;
  adjust_bar();
}

void
Scrollbar::set_total(float total)
{
  m_total = total;
  adjust_bar();
}

float
Scrollbar::get_progress() const
{
  return m_progress;
}

float
Scrollbar::get_cover() const
{
  return m_cover;
}

float
Scrollbar::get_total() const
{
  return m_total;
}

bool
Scrollbar::is_valid() const
{
  return m_total > m_cover;
}

const Control::Theme&
Scrollbar::get_current_theme() const
{
  if (m_disabled)
    return m_theme.disabled;

  if (m_dragging)
    return m_theme.active;

  if (m_rect.contains(m_mouse_pos))
    return m_theme.hover;

  if (m_parent && m_parent->get_focused_child() == this)
    return m_theme.focus;

  return m_theme.normal;
}

void
Scrollbar::adjust_bar()
{
  m_progress = Math::clamp(0.f, m_total - m_cover, m_progress);
  m_progress = std::max(0.f, m_progress);
}
