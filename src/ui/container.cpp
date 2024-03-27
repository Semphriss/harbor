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

#ifndef _HEADER_HARBOR_UI_CONTAINER_HPP

#include "ui/container.hpp"

#include "util/assert.hpp"
#include "video/drawing_context.hpp"

Container::Container(bool bufferize_draw, int layer, const Rect& rect,
                     const Control::ThemeSet& theme, Container* parent) :
  Control(layer, rect, theme, parent),
  m_children(),
  m_focus_index(-1),
  m_bufferize_draw(bufferize_draw)
{
}

bool
Container::event(const SDL_Event& event)
{
  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_TAB)
  {
    Control* ctrl = get_focused_child();
    if (ctrl)
    {
      Container* container = dynamic_cast<Container*>(ctrl);
      if (container)
      {
        return container->event(event);
      }
    }

    advance_focus(!(event.key.keysym.mod & KMOD_SHIFT));

    // Check if we just entered a subcontainer, and if so, do the same as above
    ctrl = get_focused_child();
    if (ctrl)
    {
      Container* container = dynamic_cast<Container*>(ctrl);
      if (container)
      {
        // Make sure it starts at the beginning/end according to the direction
        container->m_focus_index = -1;
        return container->event(event);
      }
    }

    return true;
  }

  bool executed_event = false;
  for (const auto& child : m_children)
  {
    assert_throw(child != nullptr, "Null control in container");
    executed_event = child->event(event);
    if (executed_event)
      break;
  }

  return executed_event;
}

void
Container::update(float dt_sec)
{
  for (const auto& child : m_children)
  {
    assert_throw(child != nullptr, "Null control in container");
    child->update(dt_sec);
  }
}

void
Container::draw(DrawingContext& context) const
{
  for (const auto& child : m_children)
  {
    assert_throw(child != nullptr, "Null control in container");
    child->draw(context);
  }
}

void
Container::set_focused(bool focused, bool call_parent)
{
  if (!focused)
  {
    for (const auto& child : m_children)
    {
      assert_throw(child != nullptr, "Null control in container");
      child->set_focused(false, false);
    }

    m_focus_index = -1;
  }
  else
  {
    if (m_focus_index >= 0 &&
        m_focus_index < static_cast<int>(m_children.size()))
    {
      m_children[m_focus_index]->set_focused(true, false);
    }
    else
    {
      assert_warn(m_focus_index == -1, "UI Container m_focus_index set to " +
                                      std::to_string(m_focus_index) + " (max " +
                                      std::to_string(m_children.size()) + ")");
    }
  }

  Control::set_focused(focused, call_parent);
}

Control*
Container::get_focused_child()
{
  if (m_focus_index >= 0 && m_focus_index < static_cast<int>(m_children.size()))
  {
    return m_children[m_focus_index].get();
  }
  else
  {
    assert_warn(m_focus_index == -1, "UI Container m_focus_index set to " +
                                     std::to_string(m_focus_index) + " (max " +
                                     std::to_string(m_children.size()) + ")");
    return nullptr;
  }
}

void
Container::set_focus(Control& child, bool focused)
{
  if (child.is_focused() != focused)
  {
    child.set_focused(focused, false);
    log_info << "Called Container::set_focus for a child directly, and the "
                "child did not already have that state; if possible, prefer "
                "calling set_focused() on the child." << std::endl;
  }

  if (focused)
  {
    int i = 0;
    for (const auto& c : m_children)
    {
      if (c.get() != &child)
      {
        c->set_focused(false, false);
      }
      else
      {
        m_focus_index = i;
      }
      i++;
    }

    set_focused(true);
  }
  else if (get_focused_child() == &child)
  {
    m_focus_index = -1;
    set_focused(false);
  }
}

void
Container::advance_focus(bool forward)
{
  bool notify_focus = true;

  if (forward)
  {
    if (m_focus_index < -1)
      m_focus_index = -1;

    if (++m_focus_index >= static_cast<int>(m_children.size()))
    {
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
  }
  else
  {
    if (m_focus_index == -1 ||
        m_focus_index > static_cast<int>(m_children.size()))
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
        m_focus_index = m_children.size() - 1;
      }
    }
  }

  if (notify_focus)
  {
    m_children[m_focus_index]->set_focused(true, true);
  }
}

void
Container::add_child(std::unique_ptr<Control> child)
{
  m_children.push_back(std::move(child));
}

#else

#include "make_unique.hpp"

template<class T, typename... Args>
T&
Container::add(Args&&... args)
{
  auto child = std::make_unique<T>(std::forward<Args>(args)..., this);
  T& ctrl = *child;
  m_children.push_back(std::move(child));
  return ctrl;
}

#endif
