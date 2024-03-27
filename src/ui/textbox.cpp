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
#include "util/math.hpp"
#include "video/drawing_context.hpp"
#include "video/font.hpp" // FIXME: Delete this!

const float Textbox::CURSOR_BLINK_TIME = 0.5f;

Textbox::Textbox(int layer, const Rect& rect, const ThemeSet& theme,
                 Container* parent) :
  Control(layer, rect, theme, parent),
  m_contents(),
  m_caret(0),
  m_caret_2(0),
  m_mouse_pos(),
  m_clicking(),
  m_scroll()
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
      m_caret = m_caret_2 = get_hit_point(m_mouse_pos);
      return is_focused();

    case SDL_MOUSEMOTION:
      m_mouse_pos = Vector(event.motion.x, event.motion.y);
      if (m_clicking && is_focused())
      {
        m_caret = get_hit_point(m_mouse_pos);
        adjust_scroll();
      }
      return m_clicking && is_focused();

    case SDL_KEYDOWN:
      if (is_focused())
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_BACKSPACE:
            backspace(false);
            break;

          case SDLK_DELETE:
            backspace(true);
            break;

          case SDLK_LEFT:
            if (m_caret > 0)
              m_caret--;

            if (!(event.key.keysym.mod & KMOD_SHIFT))
              m_caret_2 = m_caret;
            break;

          case SDLK_RIGHT:
            if (m_caret < static_cast<int>(m_contents.size()))
              m_caret++;

            if (!(event.key.keysym.mod & KMOD_SHIFT))
              m_caret_2 = m_caret;
            break;

          case SDLK_HOME:
            m_caret = 0;
            if (!(event.key.keysym.mod & KMOD_SHIFT))
              m_caret_2 = m_caret;
            break;

          case SDLK_END:
            m_caret = m_contents.size();
            if (!(event.key.keysym.mod & KMOD_SHIFT))
              m_caret_2 = m_caret;
            break;

          default:
            break;
        }

        adjust_scroll();
        return true;
      }
      return false;

    case SDL_TEXTINPUT:
      if (is_focused())
      {
        put_text(std::string(event.text.text));
        return true;
      }
      return false;

    default:
      break;
  }

  return false;
}

void
Textbox::update(float /* dt_sec */)
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
  context.get_transform().move(Vector(m_scroll, 0.f));

  // FIXME: The Font class should never have to be used directly, needs refactor
  float w1 = Font::get_font(theme.font, theme.fontsize)
                  .get_text_width(m_contents.substr(0, m_caret))
                  + contents_rect.x1;
  float w2 = Font::get_font(theme.font, theme.fontsize)
                  .get_text_width(m_contents.substr(0, m_caret_2))
                  + contents_rect.x1;

  Rect selection(w1, m_rect.y1, w2, m_rect.y2);
  selection.fix();

  if (selection.is_valid())
  {
    context.draw_filled_rect(selection, Color(1.f - theme.fg_color.r, 1.f -
                             theme.fg_color.g, 1.f - theme.fg_color.b),
                             theme.fg_blend, m_layer);
  }

  context.draw_line(Vector(w1, m_rect.y1), Vector(w1, m_rect.y2),
                    theme.fg_color, theme.fg_blend, m_layer);

  context.draw_text(m_contents,
                    (contents_rect.top_lft() + contents_rect.bot_lft()) / 2,
                    Renderer::TextAlign::MID_LEFT, theme.font, theme.fontsize,
                    theme.fg_color, theme.fg_blend, m_layer);

  context.pop_transform();
}

const std::string&
Textbox::get_contents() const
{
  return m_contents;
}

void
Textbox::put_text(const std::string& text)
{
  int i_begin = std::min(m_caret, m_caret_2);
  int i_end = std::max(m_caret, m_caret_2);

  m_contents = m_contents.substr(0, i_begin) + text + m_contents.substr(i_end);

  m_caret_2 = m_caret = i_begin + text.length();

  adjust_scroll();
}

void
Textbox::backspace(bool front)
{
  int i_begin = std::min(m_caret, m_caret_2);
  int i_end = std::max(m_caret, m_caret_2);

  if (i_begin == i_end)
  {
    if (front)
    {
      if (i_end++ >= static_cast<int>(m_contents.size()))
        return;
    }
    else
    {
      if (--i_begin < 0)
        return;
    }
  }

  m_contents = m_contents.substr(0, i_begin) + m_contents.substr(i_end);

  m_caret_2 = m_caret = i_begin;

  adjust_scroll();
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

void
Textbox::adjust_scroll()
{
  const auto& theme = get_current_theme();
  float w = m_rect.width() - theme.left.padding - theme.right.padding;

  float scroll = Font::get_font(theme.font, theme.fontsize)
                                 .get_text_width(m_contents.substr(0, m_caret));

  m_scroll = Math::clamp(scroll - w, scroll, m_scroll);
}

int
Textbox::get_hit_point(const Vector& p)
{
  const auto& theme = get_current_theme();
  float x = p.x - m_rect.x1 - theme.left.padding + m_scroll;

  int pos = 0;
  float dist = x;
  float newdist = 0.f;

  while (pos < static_cast<int>(m_contents.size()) &&
          (newdist = std::abs(Font::get_font(theme.font, theme.fontsize)
          .get_text_width(m_contents.substr(0, pos + 1)) - x)) < dist)
  {
    dist = newdist;
    pos++;
  }

  return pos;
}
