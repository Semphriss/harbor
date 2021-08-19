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

#ifndef _HEADER_HARBOR_UI_CONTAINERSCROLL_HPP
#define _HEADER_HARBOR_UI_CONTAINERSCROLL_HPP

#include "ui/container.hpp"

#include "ui/scrollbar.hpp"
#include "util/size.hpp"

class Size;

class ContainerScroll :
  public Container
{
public:
  ContainerScroll(const Size& contents_size, const ThemeSet& scroll_theme,
                  bool bufferize_draw, int layer, const Rect& rect,
                  const ThemeSet& theme, Container* parent);

  virtual bool event(const SDL_Event& event) override;
  virtual void draw(DrawingContext& context) const override;
  virtual void set_focused(bool focused, bool call_parent = true) override;

  virtual Control* get_focused_child() override;
  virtual void set_focus(Control& child, bool focused) override;
  virtual void advance_focus(bool forward = true) override;

private:
  Size m_contents_size;
  Scrollbar m_scroll_v;
  Scrollbar m_scroll_h;
  int m_selected_scrollbar;

private:
  ContainerScroll(const ContainerScroll&) = delete;
  ContainerScroll& operator=(const ContainerScroll&) = delete;
};

#endif
