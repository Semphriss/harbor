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
#define _HEADER_HARBOR_UI_LISTBOX_HPP

#include "ui/control.hpp"

#include <vector>

#include "ui/scrollbar.hpp"

template<typename T>
class Listbox :
  public Control
{
public:
  Listbox(float item_height, const ThemeSet& scroll_theme, int layer,
          const Rect& rect, const ThemeSet& theme, Container* parent);

  virtual bool event(const SDL_Event& event) override;
  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) const override;

  const T* get_selected_item() const;
  std::string get_selected_label() const;
  int get_selected_index() const;
  void add_item(std::string name, T item);
  bool remove_item(const T& item);
  bool remove_item(int index);
  void clear_items();
  void set_on_changed(std::function<void(int, const T*)> on_changed);

  void update_scrollbar_rect();

private:
  int get_index_at(const Vector& point) const;
  const T* get_item_at(const Vector& point) const;
  void adjust_scrollbar();

private:
  std::vector<std::tuple<std::string, T>> m_items;
  int m_selected;
  Scrollbar m_scrollbar;
  float m_item_height;
  Vector m_mouse_pos;
  /** 1 = left; 2 = right; 4 = middle. All OR'd together. */
  int m_mouse_button_pressed;
  std::function<void(int, const T*)> m_on_changed;

private:
  Listbox(const Listbox&) = delete;
  Listbox& operator=(const Listbox&) = delete;
};

#include "ui/listbox.cpp"

#endif
