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

#include "video/drawing_context.hpp"

#include "make_unique.hpp"

#include "video/renderer.hpp"
#include "video/texture.hpp"

void
DrawingContext::FillRectRequest::render(Renderer& renderer) const
{
  renderer.draw_filled_rect(m_rect, m_color, m_blend);
}

DrawingContext::TextureRequest::TextureRequest(const Texture& texture) :
  m_texture(texture)
{
}

void
DrawingContext::TextureRequest::render(Renderer& renderer) const
{
  renderer.draw_texture(m_texture, m_srcrect, m_dstrect, m_color, m_blend);
}

DrawingContext::DrawingContext(Renderer& renderer) :
  m_renderer(renderer)
{
}

void
DrawingContext::draw_filled_rect(const Rect& rect, const Color& color,
                                 const Renderer::Blend& blend, int layer)
{
  auto req = std::make_unique<FillRectRequest>();
  req->m_color = color;
  req->m_blend = blend;
  req->m_rect = rect;

  m_requests[layer].push_back(std::move(req));
}

void
DrawingContext::draw_texture(const Texture& texture,
                             const Rect& srcrect, const Rect& dstrect,
                             const Color& color, const Renderer::Blend& blend,
                             int layer)
{
  auto req = std::make_unique<TextureRequest>(texture);
  req->m_color = color;
  req->m_blend = blend;
  req->m_srcrect = srcrect;
  req->m_dstrect = dstrect;

  m_requests[layer].push_back(std::move(req));
}

void
DrawingContext::render(Texture* texture) const
{
  m_renderer.start_draw(texture);

  for (const auto& reqs : m_requests)
  {
    for (const auto& req : reqs.second)
    {
      req->render(m_renderer);
    }
  }

  m_renderer.end_draw();
}

void
DrawingContext::clear()
{
  m_requests.clear();
}
