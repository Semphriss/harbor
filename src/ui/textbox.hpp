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

#ifndef _HEADER_HARBOR_UI_TEXTBOX_HPP
#define _HEADER_HARBOR_UI_TEXTBOX_HPP

#include "ui/control.hpp"

#include <functional>

class Textbox :
  public Control
{
public:
  static const float CURSOR_BLINK_TIME;

public:
  Textbox(int layer, const Rect& rect, const ThemeSet& theme,
          Container* parent);
  virtual ~Textbox() override;

  virtual bool event(const SDL_Event& event) override;
  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) const override;

  const std::string& get_contents() const;

  /** Puts text at given caret location, replacing any selected text */
  void put_text(const std::string& text);
  /**
   * Delete selected text if any, else delete the character before the caret if
   * @p front is false or after the caret if @p front is true
   */
  void backspace(bool front = false);

private:
  const Theme& get_current_theme() const;
  /** Sets m_scoll to a value which makes the principal caret visible */
  void adjust_scroll();
  /** Returns the caret position corresponding to the on-screen point given */
  int get_hit_point(const Vector& p);

private:
  std::string m_contents;
  int m_caret;
  int m_caret_2;
  Vector m_mouse_pos;
  bool m_clicking;
  float m_scroll;

private:
  Textbox(const Textbox&) = delete;
  Textbox& operator=(const Textbox&) = delete;
};

#endif

/* EOF */
