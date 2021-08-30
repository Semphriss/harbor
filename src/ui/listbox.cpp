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

#ifndef _HEADER_HARBOR_UI_LISTBOX_HPP

#include "ui/listbox.hpp"

#else

#include <cmath>

#include "ui/container.hpp"
#include "util/assert.hpp"
#include "util/math.hpp"
#include "video/drawing_context.hpp"

template<typename T>
Listbox<T>::Listbox(float item_height, const ThemeSet& scroll_theme, int layer,
                    const Rect& rect, const ThemeSet& theme,
                    Container* parent) :
  Control(layer, rect, theme, parent),
  m_items(),
  m_selected(-1),
  m_scrollbar(nullptr, m_rect.height(), 0.f, false, 1, m_layer,
              m_rect.with_x1(m_rect.x2 - 10.f), scroll_theme, nullptr),
  m_item_height(item_height),
  m_mouse_pos(0, 0),
  m_mouse_button_pressed(0)
{
}

template<typename T>
bool
Listbox<T>::event(const SDL_Event& event)
{
  if (m_scrollbar.event(event))
    return true;

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
      set_focused(on_this, true);

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
      set_focused(on_this, true);
      m_scrollbar.set_focused(false);

      if ((m_mouse_button_pressed & 1) && is_focused())
      {
        m_selected = get_index_at(m_mouse_pos);
        if (m_on_changed)
          m_on_changed(m_selected, get_selected_item());
      }

      return on_this;
    }

    case SDL_KEYDOWN:
      if (is_focused())
      {
        switch (event.key.keysym.sym)
        {
          case SDLK_DOWN:
            m_selected = (m_selected + 1) % m_items.size();
            adjust_scrollbar();
            if (m_on_changed)
              m_on_changed(m_selected, get_selected_item());
            break;

          case SDLK_UP:
            m_selected = (m_selected + m_items.size() - 1) % m_items.size();
            adjust_scrollbar();
            if (m_on_changed)
              m_on_changed(m_selected, get_selected_item());
            break;

          default:
            break;
        }
      }
      return is_focused();

    case SDL_MOUSEMOTION:
      m_mouse_pos = Vector(event.motion.x, event.motion.y);
      return false;

    case SDL_MOUSEWHEEL:
      if (m_rect.contains(m_mouse_pos))
      {
        m_scrollbar.set_progress(m_scrollbar.get_progress() - event.wheel.y * 10);
        return true;
      }
      return false;

    default:
      break;
  }

  return false;
}

template<typename T>
void
Listbox<T>::update(float dt_sec)
{
}

template<typename T>
void
Listbox<T>::draw(DrawingContext& context) const
{
  context.draw_filled_rect(m_rect.grown(1).moved({0,0}), Color(0,0,0,0.1), Renderer::Blend::BLEND, m_layer);
  context.draw_filled_rect(m_rect.grown(1).moved({0,-1}), Color(0,0,0,0.1), Renderer::Blend::BLEND, m_layer);
  context.draw_filled_rect(m_rect.grown(1).moved({0,-2}), Color(0,0,0,0.1), Renderer::Blend::BLEND, m_layer);
  context.draw_filled_rect(m_rect.grown(1).moved({0,-3}), Color(0,0,0,0.1), Renderer::Blend::BLEND, m_layer);
  context.draw_filled_rect(m_rect, m_theme.normal.bg_color,
                           m_theme.normal.bg_blend, m_layer);

  context.push_transform();
  context.get_transform().clip(m_rect);
  context.get_transform().move(Vector(0, m_scrollbar.get_progress()));

  const T* selected = get_selected_item();
  const T* hovered = get_item_at(m_mouse_pos);
  int i = 0;
  for (const auto& item : m_items)
  {
    auto theme = m_theme.normal;

    if (m_disabled)
    {
      theme = m_theme.disabled;
    }
    else if (&std::get<1>(item) == hovered && (m_mouse_button_pressed & 1))
    {
      theme = m_theme.active;
    }
    else if (&std::get<1>(item) == selected)
    {
      theme = m_theme.focus;
    }
    else if (&std::get<1>(item) == hovered)
    {
      theme = m_theme.hover;
    }

    Rect r = m_rect.with_y2(m_rect.y1 + m_item_height)
                   .moved(Vector(0, -m_item_height * i));

    context.draw_filled_rect(r, theme.bg_color, theme.bg_blend, m_layer);
    context.draw_text(std::get<0>(item), r.mid(), Renderer::TextAlign::CENTER,
                      theme.font, theme.fontsize, theme.fg_color,
                      theme.fg_blend, m_layer);

    i++;
  }

  context.pop_transform();

  if (m_scrollbar.is_valid())
  {
    m_scrollbar.draw(context);
  }
}

template<typename T>
const T*
Listbox<T>::get_selected_item() const
{
  if (m_selected >= 0 && m_selected < m_items.size())
  {
    return &std::get<1>(m_items[m_selected]);
  }
  else
  {
    assert_warn(m_selected == -1,
                "Listbox selection is invalid: " + std::to_string(m_selected));

    return nullptr;
  }
}

template<typename T>
std::string
Listbox<T>::get_selected_label() const
{
  if (m_selected >= 0 && m_selected < m_items.size())
  {
    return std::get<0>(m_items[m_selected]);
  }
  else
  {
    assert_warn(m_selected == -1,
                "Listbox selection is invalid: " + std::to_string(m_selected));

    return "";
  }
}

template<typename T>
int
Listbox<T>::get_selected_index() const
{
  return m_selected;
}

template<typename T>
void
Listbox<T>::add_item(std::string name, T item)
{
  m_items.push_back(std::make_tuple(name, item));
  m_scrollbar.set_total(m_item_height * m_items.size());
}

template<typename T>
bool
Listbox<T>::remove_item(const T& item)
{
  auto it = std::find(m_items.begin(), m_items.end(), item);
  if (it != m_items.end())
  {
    m_items.erase(it);

    if (m_selected >= m_items.size())
    {
      m_selected = -1;
      if (m_on_changed)
        m_on_changed(m_selected, nullptr);
    }

    return true;
  }

  return false;
}

template<typename T>
bool
Listbox<T>::remove_item(int index)
{
  if (index < 0 || index >= m_items.size())
    return false;

  auto it = m_items.begin();
  std::advance(it, index);
  m_items.erase(it);

  if (m_selected >= m_items.size())
  {
    m_selected = -1;
    if (m_on_changed)
      m_on_changed(m_selected, nullptr);
  }

  return true;
}

template<typename T>
void
Listbox<T>::clear_items()
{
  m_items.clear();
  m_scrollbar.set_total(0.f);
}

template<typename T>
void
Listbox<T>::set_on_changed(std::function<void(int, const T*)> on_changed)
{
  m_on_changed = on_changed;
}

template<typename T>
int
Listbox<T>::get_index_at(const Vector& point) const
{
  if (!m_rect.contains(point))
    return -1;

  float hit = point.y - m_rect.y1 + m_scrollbar.get_progress();
  int index = static_cast<int>(floor(hit  / m_item_height));

  if (index < 0 || index >= m_items.size())
    index = -1;

  return index;
}

template<typename T>
const T*
Listbox<T>::get_item_at(const Vector& point) const
{
  int i = get_index_at(point);

  if (i >= 0 && i < m_items.size())
  {
    return &std::get<1>(m_items[i]);
  }
  else
  {
    return nullptr;
  }
}

template<typename T>
void
Listbox<T>::adjust_scrollbar()
{
  float progress = m_scrollbar.get_progress();

  float p_max = static_cast<float>(m_selected) * m_item_height;
  float p_min = p_max + m_item_height - m_rect.height();

  m_scrollbar.set_progress(Math::clamp(p_min, p_max, progress));
}

#endif
