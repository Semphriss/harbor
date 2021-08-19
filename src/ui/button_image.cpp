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

#include "ui/button_image.hpp"

#include "video/drawing_context.hpp"
#include "video/window.hpp"

ButtonImage::ButtonImage(const std::string image, Scaling scaling,
                         std::function<void(int)> on_click, int btnmask, 
                         bool onbtnup, int layer, const Rect& rect,
                         const ThemeSet& theme, Container* parent) :
  Button(on_click, btnmask, onbtnup, layer, rect, theme, parent),
  m_image(image),
  m_scaling(scaling)
{
}

void
ButtonImage::draw(DrawingContext& context) const
{
  Button::draw(context);

  const auto& theme = get_current_theme();
  Texture& image = context.get_renderer().get_window().load_texture(m_image);

  Rect img_rect = m_rect;
  img_rect.x1 += theme.left.padding;
  img_rect.y1 += theme.top.padding;
  img_rect.x2 -= theme.right.padding;
  img_rect.y2 -= theme.bottom.padding;

  Size s = image.get_size();
  switch(m_scaling)
  {
    case Scaling::NONE:
      break;

    case Scaling::STRETCH:
      s = img_rect.size();
      break;

    case Scaling::CONTAIN:
      s *= std::min(img_rect.height() / image.get_size().h,
                    img_rect.width() / image.get_size().w);
      break;

    case Scaling::COVER:
    {
      s *= std::max(img_rect.height() / image.get_size().h,
                    img_rect.width() / image.get_size().w);
    }
      break;
  };

  context.push_transform();
  context.get_transform().clip(img_rect);

  context.draw_texture(image, Rect(Vector(), image.get_size()),
                       Rect(img_rect.mid() - Vector(s / 2), s), 0.f,
                       Color(1.f, 1.f, 1.f), theme.fg_blend, m_layer);

  context.pop_transform();
}
