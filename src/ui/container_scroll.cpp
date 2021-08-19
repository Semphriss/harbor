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

#include "ui/container_scroll.hpp"

#include "util/assert.hpp"
#include "video/drawing_context.hpp"

ContainerScroll::ContainerScroll(const Size& contents_size,
                                 const ThemeSet& scroll_theme,
                                 bool bufferize_draw, int layer,
                                 const Rect& rect, const ThemeSet& theme,
                                 Container* parent) :
  Container(bufferize_draw, layer, rect, theme, parent),
  m_contents_size(contents_size),
  m_scroll_v(nullptr, rect.height(), contents_size.h, false, 1, layer + 1,
             rect.with_x1(rect.x2 - 10.f).set_y2(rect.y2 - 10.f), scroll_theme,
             this),
  m_scroll_h(nullptr, rect.width(), contents_size.w, true, 1, layer + 1,
             rect.with_y1(rect.y2 - 10.f).set_x2(rect.x2 - 10.f), scroll_theme,
             this),
  m_selected_scrollbar(0)
{
}

bool
ContainerScroll::event(const SDL_Event& event)
{
  if (m_scroll_h.event(event) || m_scroll_v.event(event))
    return true;

  SDL_Event e = event;
  switch (event.type)
  {
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
      if (!m_rect.contains(Vector(e.button.x, e.button.y)))
        return false;
      e.button.x += m_scroll_h.get_progress();
      e.button.y += m_scroll_v.get_progress();
      break;

    case SDL_MOUSEMOTION:
      if (!m_rect.contains(Vector(e.motion.x, e.motion.y)))
        return false;
      e.motion.x += m_scroll_h.get_progress();
      e.motion.y += m_scroll_v.get_progress();
      break;

    default:
      break;
  }

  return Container::event(e);
}

void
ContainerScroll::draw(DrawingContext& context) const
{
  context.push_transform();

  context.get_transform().clip(m_rect);
  context.get_transform().move(Vector(m_scroll_h.get_progress(),
                                      m_scroll_v.get_progress()));

  Container::draw(context);

  context.pop_transform();

  m_scroll_h.draw(context);
  m_scroll_v.draw(context);
}

void
ContainerScroll::set_focused(bool focused, bool call_parent)
{
  if (!focused)
  {
    m_scroll_h.set_focused(false, false);
    m_scroll_v.set_focused(false, false);
    m_selected_scrollbar = 0;
  }
  else
  {
    switch(m_selected_scrollbar)
    {
      case 1:
        m_scroll_h.set_focused(true, false);
        break;

      case 2:
        m_scroll_v.set_focused(true, false);
        break;

      default:
        break;
    }
  }

  Container::set_focused(focused, call_parent);
}

Control*
ContainerScroll::get_focused_child()
{
  switch(m_selected_scrollbar)
  {
    case 0:
      return Container::get_focused_child();

    case 1:
      return &m_scroll_h;

    case 2:
      return &m_scroll_v;

    default:
      log_warn << "Invalid value for m_selected_scrollbar: "
               << m_selected_scrollbar << std::endl;
      return nullptr;
  }
}

void
ContainerScroll::set_focus(Control& child, bool focused)
{
  if (child.is_focused() != focused)
  {
    child.set_focused(focused, false);
    log_info << "Called Container::set_focus for a child directly, and the "
                "child did not already have that state; if possible, prefer "
                "calling set_focused() on the child." << std::endl;
  }

  if (&child == &m_scroll_h)
  {
    m_scroll_h.set_focused(true, false);
    m_scroll_v.set_focused(false, false);
  }
  else if (&child == &m_scroll_v)
  {
    m_scroll_h.set_focused(false, false);
    m_scroll_v.set_focused(true, false);
  }
  else
  {
    if (focused)
    {
      m_scroll_h.set_focused(false, false);
      m_scroll_v.set_focused(false, false);
    }

    Container::set_focus(child, focused);
  }
}

void
ContainerScroll::advance_focus(bool forward)
{
  bool notify_focus = true;

  if (forward)
  {
    if (m_selected_scrollbar == 1)
    {
      m_selected_scrollbar = 2;
    }
    else if (m_selected_scrollbar == 2)
    {
      m_selected_scrollbar = 0;

      if (m_parent)
      {
        notify_focus = false;
        set_focused(false, false);
        m_parent->advance_focus(true);
      }
      else
      {
        m_focus_index = 0;
      }
    }
    else
    {
      if (m_focus_index < -1)
        m_focus_index = -1;

      if (++m_focus_index >= m_children.size())
      {
        m_selected_scrollbar = 1;
      }
    }
  }
  else
  {
    if (m_selected_scrollbar == 2)
    {
      m_selected_scrollbar = 1;
    }
    else
    {
      if (m_selected_scrollbar == 1)
      {
        m_selected_scrollbar = 0;
      }

      if (m_focus_index == -1 || m_focus_index > m_children.size())
        m_focus_index = m_children.size();

      if (--m_focus_index < 0)
      {
        if (m_parent)
        {
          notify_focus = false;
          set_focused(false, false);
          m_parent->advance_focus(false);
        }
        else
        {
          m_focus_index = m_children.size();
          m_selected_scrollbar = 2;
        }
      }
    }
  }

  if (notify_focus)
  {
    switch(m_selected_scrollbar)
    {
      case 0:
        m_children[m_focus_index]->set_focused(true, true);
        break;

      case 1:
        m_scroll_h.set_focused(true, true);
        break;

      case 2:
        m_scroll_v.set_focused(true, true);
        break;

      default:
        log_warn << "Invalid value for m_selected_scrollbar: "
                 << m_selected_scrollbar << std::endl;
        break;
    }
  }
}
