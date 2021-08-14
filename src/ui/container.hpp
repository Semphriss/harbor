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
#define _HEADER_HARBOR_UI_CONTAINER_HPP

#include "ui/control.hpp"

#include <memory>
#include <vector>

class DrawingContext;
union SDL_Event;

class Container :
  public Control
{
public:
  Container(bool bufferize_draw, int layer, const Rect& rect,
            const ThemeSet& theme, Container* parent);

  virtual void event(const SDL_Event& event) override;
  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) const override;

  virtual void set_focused(bool focused, bool call_parent = true) override;

  Control* get_focused_child() const;
  void add_child(std::unique_ptr<Control> child);
  void set_focus(Control& child, bool focused);
  void advance_focus(bool forward = true);

  template<class T, typename... Args> T& add(Args&&... args);

private:
  std::vector<std::unique_ptr<Control>> m_children;
  int m_focus_index;
  bool m_bufferize_draw;

private:
  Container(const Container&) = delete;
  Container& operator=(const Container&) = delete;
};

#include "ui/container.cpp"

#endif
