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

#ifndef _HEADER_HARBOR_UI_SCROLLBAR_HPP
#define _HEADER_HARBOR_UI_SCROLLBAR_HPP

#include "ui/control.hpp"

#include <functional>

class Scrollbar :
  public Control
{
public:
  Scrollbar(std::function<void(float)> on_scroll, float cover, float total,
            bool horizontal, int btnmask, int layer, const Rect& rect,
            const ThemeSet& theme, Container* parent);

  virtual bool event(const SDL_Event& event) override;
  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) const override;

  void set_progress(float progress);
  void set_cover(float cover);
  void set_total(float total);

  float get_progress() const;
  float get_cover() const;
  float get_total() const;
  bool is_valid() const;

private:
  const Theme& get_current_theme() const;
  void adjust_bar();

private:
  std::function<void(float)> m_on_scroll;
  Vector m_mouse_pos;
  /** 1 = left; 2 = right; 4 = middle. All OR'd together. */
  int m_mouse_button_pressed;
  int m_btnmask;
  float m_progress;
  float m_cover;
  float m_total;
  bool m_horizontal;
  bool m_dragging;

private:
  Scrollbar(const Scrollbar&) = delete;
  Scrollbar& operator=(const Scrollbar&) = delete;
};

#endif
