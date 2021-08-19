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

#ifndef _HEADER_HARBOR_UI_CONTROL_HPP
#define _HEADER_HARBOR_UI_CONTROL_HPP

#include <string>

#include "util/color.hpp"
#include "util/rect.hpp"
#include "util/vector.hpp"

#include "video/renderer.hpp"

class Container;
class DrawingContext;
union SDL_Event;

class Control
{
public:
  struct Border
  {
    Color color;
    Renderer::Blend blend;
    float margin;
    float thickness;
    float padding;
  };

  struct Theme
  {
    Color bg_color;
    Color fg_color;
    Renderer::Blend bg_blend;
    Renderer::Blend fg_blend;
    Border top;
    Border left;
    Border right;
    Border bottom;
    float round_corners;
    std::string font;
    int fontsize;
  };

  struct ThemeSet
  {
    Theme normal;
    Theme hover;
    Theme focus;
    Theme active;
    Theme disabled;
  };

public:
  Control(int layer, const Rect& rect, const ThemeSet& theme, Container* parent);
  virtual ~Control();

  virtual bool event(const SDL_Event& event) = 0;
  virtual void update(float dt_sec) = 0;
  virtual void draw(DrawingContext& context) const = 0;

  virtual void set_focused(bool focused, bool call_parent = true);

  ThemeSet& get_theme();
  Rect& get_rect();
  const ThemeSet& get_theme() const;
  const Rect& get_rect() const;
  bool is_focused() const;

  Container* get_parent() const;

protected:
  Rect m_rect;
  ThemeSet m_theme;
  Container* m_parent;
  bool m_focused;
  bool m_disabled;
  int m_layer;

private:
  Control(const Control&) = delete;
  Control& operator=(const Control&) = delete;
};

#endif
