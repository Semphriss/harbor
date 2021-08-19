//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "ui/textbox.hpp"

#include "ui/container.hpp"
#include "util/log.hpp"
#include "video/drawing_context.hpp"
#include "video/font.hpp" // FIXME: Delete this!

const float Textbox::CURSOR_BLINK_TIME = 0.5f;

Textbox::Textbox(int layer, const Rect& rect, const ThemeSet& theme,
                 Container* parent) :
  Control(layer, rect, theme, parent)
{
}

Textbox::~Textbox()
{
}

bool
Textbox::event(const SDL_Event& event)
{
  switch (event.type)
  {
    case SDL_MOUSEBUTTONUP:
      m_clicking = false;
      return false;

    case SDL_MOUSEBUTTONDOWN:
      set_focused(m_rect.contains(m_mouse_pos));
      m_clicking = is_focused();
      return is_focused();

    case SDL_MOUSEMOTION:
      m_mouse_pos = Vector(event.motion.x, event.motion.y);
      return false;

    case SDL_KEYDOWN:
      if (is_focused())
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_BACKSPACE:
            if (!m_contents.empty())
              m_contents.pop_back();
            break;

          case SDLK_LEFT:
            if (m_caret > 0)
              m_caret--;

            if (!(event.key.keysym.mod & KMOD_SHIFT))
              m_caret_2 = m_caret;
            break;

          case SDLK_RIGHT:
            if (m_caret < m_contents.size() - 1)
              m_caret++;

            if (!(event.key.keysym.mod & KMOD_SHIFT))
              m_caret_2 = m_caret;
            break;

          default:
            break;
        }

        return true;
      }
      return false;

    case SDL_TEXTINPUT:
      if (is_focused())
      {
        m_contents += std::string(event.text.text);
        return true;
      }
      return false;

    default:
      break;
  }

  return false;
}

void
Textbox::update(float dt_sec)
{
}

void
Textbox::draw(DrawingContext& context) const
{
  const auto& theme = get_current_theme();
  context.draw_filled_rect(m_rect, theme.bg_color, theme.bg_blend, m_layer);

  Rect contents_rect = m_rect;
  contents_rect.x1 += theme.left.padding;
  contents_rect.y1 += theme.top.padding;
  contents_rect.x2 -= theme.right.padding;
  contents_rect.y2 -= theme.bottom.padding;

  context.push_transform();
  context.get_transform().clip(contents_rect);

  context.draw_text(m_contents,
                    (contents_rect.top_lft() + contents_rect.bot_lft()) / 2,
                    Renderer::TextAlign::MID_LEFT, theme.font, theme.fontsize,
                    theme.fg_color, theme.fg_blend, m_layer);

  // FIXME: The Font class should never have to be used directly, needs refactor
  float w = Font::get_font(theme.font, theme.fontsize)
                  .get_text_width(m_contents) + contents_rect.x1;

  context.draw_line(Vector(w, m_rect.y1), Vector(w, m_rect.y2), theme.fg_color,
                    theme.fg_blend, m_layer);

  context.pop_transform();
}

const std::string&
Textbox::get_contents() const
{
  return m_contents;
}

const Control::Theme&
Textbox::get_current_theme() const
{
  if (m_disabled)
    return m_theme.disabled;

  if (m_rect.contains(m_mouse_pos))
  {
    if (m_clicking)
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
