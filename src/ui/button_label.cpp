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

#include "ui/button_label.hpp"

#include "video/drawing_context.hpp"

ButtonLabel::ButtonLabel(const std::string label,
                         std::function<void(int)> on_click, int btnmask, 
                         bool onbtnup, int layer, const Rect& rect,
                         const ThemeSet& theme, Container* parent) :
  Button(on_click, btnmask, onbtnup, layer, rect, theme, parent),
  m_label(label)
{
}

void
ButtonLabel::draw(DrawingContext& context) const
{
  Button::draw(context);

  const auto& theme = get_current_theme();
  context.draw_text(m_label, m_rect.mid(), Renderer::TextAlign::CENTER,
                    theme.font, theme.fontsize, theme.fg_color, theme.fg_blend,
                    0);
}
