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

#ifndef _HEADER_HARBOR_UI_BUTTON_HPP
#define _HEADER_HARBOR_UI_BUTTON_HPP

#include "ui/control.hpp"

#include <functional>

class Container;
class DrawingContext;
union SDL_Event;

class Button :
  public Control
{
public:
  Button(std::function<void(int)> on_click, int btnmask, bool onbtnup,
         int layer, const Rect& rect, const ThemeSet& theme, Container* parent);

  virtual void event(const SDL_Event& event) override;
  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) const override;

protected:
  const Theme& get_current_theme() const;

protected:
  std::function<void(int)> m_on_click;
  Vector m_mouse_pos;
  /** 1 = left; 2 = right; 4 = middle. All OR'd together. */
  int m_mouse_button_pressed;
  int m_btnmask;
  bool m_onbtnup;

private:
  Button(const Button&) = delete;
  Button& operator=(const Button&) = delete;
};

#endif
