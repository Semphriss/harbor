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

DrawingContext::Transform::Transform() :
  m_offset(),
  m_scale(1.f, 1.f)
{
}

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
  renderer.draw_texture(m_texture, m_srcrect, m_dstrect, m_angle, m_color,
                        m_blend);
}

void
DrawingContext::TextRequest::render(Renderer& renderer) const
{
  renderer.draw_text(m_text, m_pos, m_align, m_font, m_size, m_color, m_blend);
}

DrawingContext::DrawingContext(Renderer& renderer) :
  m_renderer(renderer)
{
  m_transform_stack.push_back(Transform());
}

void
DrawingContext::draw_filled_rect(const Rect& rect, const Color& color,
                                 const Renderer::Blend& blend, int layer)
{
  auto req = std::make_unique<FillRectRequest>();
  req->m_color = color;
  req->m_blend = blend;
  req->m_rect = rect.moved(get_transform().m_offset) * get_transform().m_scale;

  m_requests[layer].push_back(std::move(req));
}

void
DrawingContext::draw_texture(const Texture& texture,
                             const Rect& srcrect, const Rect& dstrect,
                             float angle, const Color& color,
                             const Renderer::Blend& blend, int layer)
{
  auto req = std::make_unique<TextureRequest>(texture);
  req->m_color = color;
  req->m_blend = blend;
  req->m_srcrect = srcrect;
  req->m_dstrect = dstrect.moved(get_transform().m_offset) *
                   get_transform().m_scale;
  req->m_angle = angle;

  m_requests[layer].push_back(std::move(req));
}

void
DrawingContext::draw_texture(const std::shared_ptr<Texture>& texture,
                             const Rect& srcrect, const Rect& dstrect,
                             float angle, const Color& color,
                             const Renderer::Blend& blend, int layer)
{
  auto req = std::make_unique<TextureRequest>(*texture);
  req->m_color = color;
  req->m_blend = blend;
  req->m_srcrect = srcrect;
  req->m_dstrect = dstrect.moved(get_transform().m_offset) *
                   get_transform().m_scale;
  req->m_angle = angle;
  req->m_texture_ptr = texture;

  m_requests[layer].push_back(std::move(req));
}

void
DrawingContext::draw_text(const std::string& text, const Vector& pos,
                Renderer::TextAlign align, const std::string& fontfile,
                int size, const Color& color, const Renderer::Blend& blend,
                int layer)
{
  auto req = std::make_unique<TextRequest>();
  req->m_color = color;
  req->m_blend = blend;
  req->m_align = align;
  req->m_font = fontfile;
  req->m_pos = pos + get_transform().m_offset;
  req->m_size = size;
  req->m_text = text;

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

void
DrawingContext::push_transform()
{
  m_transform_stack.push_back(Transform(m_transform_stack.back()));
}

void
DrawingContext::pop_transform()
{
  m_transform_stack.pop_back();
}

DrawingContext::Transform&
DrawingContext::get_transform()
{
  return m_transform_stack.back();
}

Renderer&
DrawingContext::get_renderer() const
{
  return m_renderer;
}
