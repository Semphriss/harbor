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

#include <cmath>

#include "make_unique.hpp"

#include "util/log.hpp"
#include "video/renderer.hpp"
#include "video/texture.hpp"
#include "video/window.hpp"

Rect
DrawingContext::clip_src_rect(const Rect& src, const Rect& dst,
                              const Rect& clip)
{
  Rect clipped = dst.clipped(clip);

  if (!clipped.is_valid())
    return Rect();

  return Rect((clipped.x1 - dst.x1) / dst.width() * src.width() + src.x1,
              (clipped.y1 - dst.y1) / dst.height() * src.height() + src.y1,
              (clipped.x2 - dst.x1) / dst.width() * src.width() + src.x1,
              (clipped.y2 - dst.y1) / dst.height() * src.height() + src.y1);
}

DrawingContext::Transform::Transform() :
  m_offset(),
  m_scale(1.f, 1.f),
  m_clip(-HUGE_VALF, -HUGE_VALF, HUGE_VALF, HUGE_VALF),
  m_color(1.f, 1.f, 1.f, 1.f)
{
}

void
DrawingContext::Transform::move(const Vector& offset)
{
  m_offset -= offset * m_scale;
}

void
DrawingContext::Transform::scale(const Size& scale)
{
  m_scale *= scale;
}

void
DrawingContext::Transform::clip(const Rect& rect)
{
  m_clip.clip(Rect(rect.top_lft() * m_scale - m_offset, rect.size() * m_scale));
}

void
DrawingContext::Transform::color(const Color& color)
{
  m_color *= color;
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
  renderer.draw_text(m_text, m_pos, m_clip, m_align, m_font, m_size, m_color,
                     m_blend);
}

void
DrawingContext::LineRequest::render(Renderer& renderer) const
{
  renderer.draw_line(m_p1, m_p2, m_color, m_blend);
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
  req->m_color = color * get_transform().m_color;
  req->m_blend = blend;
  req->m_rect = (rect.moved(get_transform().m_offset) * get_transform().m_scale)
                 .clipped(get_transform().m_clip);

  if (!req->m_rect.is_valid() || req->m_rect.is_null())
  {
    log_info << "Not rendering empty filled rect" << std::endl;
    return;
  }

  m_requests[layer].push_back(std::move(req));
}

void
DrawingContext::draw_texture(const Texture& texture,
                             const Rect& srcrect, const Rect& dstrect,
                             float angle, const Color& color,
                             const Renderer::Blend& blend, int layer)
{
  Rect dst = (dstrect.moved(get_transform().m_offset) * get_transform().m_scale);

  auto req = std::make_unique<TextureRequest>(texture);
  req->m_color = color * get_transform().m_color;
  req->m_blend = blend;
  req->m_srcrect = clip_src_rect(srcrect, dst, get_transform().m_clip);
  req->m_dstrect = dst.clipped(get_transform().m_clip);
  req->m_angle = angle;

  if (!req->m_srcrect.is_valid() || req->m_srcrect.is_null() ||
      !req->m_dstrect.is_valid() || req->m_dstrect.is_null())
  {
    log_info << "Not rendering empty texture" << std::endl;
    return;
  }

  m_requests[layer].push_back(std::move(req));
}

void
DrawingContext::draw_texture(const std::shared_ptr<Texture>& texture,
                             const Rect& srcrect, const Rect& dstrect,
                             float angle, const Color& color,
                             const Renderer::Blend& blend, int layer)
{
  Rect dst = (dstrect.moved(get_transform().m_offset) * get_transform().m_scale);

  auto req = std::make_unique<TextureRequest>(*texture);
  req->m_color = color * get_transform().m_color;
  req->m_blend = blend;
  req->m_srcrect = clip_src_rect(srcrect, dst, get_transform().m_clip);
  req->m_dstrect = dst.clipped(get_transform().m_clip);
  req->m_angle = angle;
  req->m_texture_ptr = texture;

  if (!req->m_srcrect.is_valid() || req->m_srcrect.is_null() ||
      !req->m_dstrect.is_valid() || req->m_dstrect.is_null())
  {
    log_info << "Not rendering empty custom texture" << std::endl;
    return;
  }

  m_requests[layer].push_back(std::move(req));
}

void
DrawingContext::draw_text(const std::string& text, const Vector& pos,
                          Renderer::TextAlign align,
                          const std::string& fontfile, int size,
                          const Color& color, const Renderer::Blend& blend,
                          bool border, int layer)
{
  if (border)
  {
    for (int x = -1; x <= 1; x += 1)
    {
      for (int y = -1; y <= 1; y += 1)
      {
        auto req = std::make_unique<TextRequest>();
        req->m_color = Color();
        req->m_blend = blend;
        req->m_align = align;
        req->m_font = fontfile;
        req->m_pos = pos - get_transform().m_offset + Vector(x, y);
        req->m_size = size;
        req->m_text = text;
        req->m_clip = get_transform().m_clip;

        m_requests[layer].push_back(std::move(req));
      }
    }

    auto req = std::make_unique<TextRequest>();
    req->m_color = Color(0.f, 0.f, 0.f, .75f);
    req->m_blend = blend;
    req->m_align = align;
    req->m_font = fontfile;
    req->m_pos = pos - get_transform().m_offset + Vector(2.f, 2.f);
    req->m_size = size;
    req->m_text = text;
    req->m_clip = get_transform().m_clip;

    m_requests[layer].push_back(std::move(req));
  }

  auto req = std::make_unique<TextRequest>();
  req->m_color = color * get_transform().m_color;
  req->m_blend = blend;
  req->m_align = align;
  req->m_font = fontfile;
  req->m_pos = pos - get_transform().m_offset;
  req->m_size = size;
  req->m_text = text;
  req->m_clip = get_transform().m_clip;

  m_requests[layer].push_back(std::move(req));
}

void
DrawingContext::draw_line(const Vector& p1, const Vector& p2,
                          const Color& color, const Renderer::Blend& blend,
                          int layer)
{
  auto req = std::make_unique<LineRequest>();
  req->m_color = color * get_transform().m_color;
  req->m_blend = blend;
  auto pts = get_transform().m_clip.clip_line(p1 * get_transform().m_scale -
                                                      get_transform().m_offset,
                                              p2 * get_transform().m_scale -
                                                      get_transform().m_offset);
  req->m_p1 = pts.first;
  req->m_p2 = pts.second;

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
