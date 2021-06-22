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

#ifndef _HEADER_HARBOR_VIDEO_DRAWINGCONTEXT_HPP
#define _HEADER_HARBOR_VIDEO_DRAWINGCONTEXT_HPP

#include <vector>
#include <map>
#include <memory>

#include "video/renderer.hpp"
#include "util/color.hpp"
#include "util/rect.hpp"
#include "util/vector.hpp"

/**
 * Class that acts as a buffer to collect drawing requests to be performed on a
 * Renderer. The buffering allows layering and separation.
 * 
 * @see Renderer
 */
class DrawingContext final
{
public:
  /**
   * Holds the data to perform a drawing request on a Renderer.
   */
  class DrawRequest
  {
  public:
    DrawRequest() = default;
    virtual ~DrawRequest() = default;

    virtual void render(Renderer& renderer) const = 0;

  public:
    Color m_color;
    Renderer::Blend m_blend;
  };

  /**
   * Holds the data to perform a FillRect request on a Renderer.
   */
  class FillRectRequest final :
    public DrawRequest
  {
  public:
    FillRectRequest() = default;

    virtual void render(Renderer& renderer) const override;

  public:
    Rect m_rect;
  };

  /**
   * Holds the data to perform a Texture request on a Renderer.
   */
  class TextureRequest final :
    public DrawRequest
  {
  public:
    TextureRequest(const Texture& texture);

    virtual void render(Renderer& renderer) const override;

  public:
    const Texture& m_texture;
    std::shared_ptr<Texture> m_texture_ptr;
    Rect m_srcrect, m_dstrect;
    float m_angle;
  };

  /**
   * Holds the data to perform a Text request on a Renderer.
   */
  class TextRequest final :
    public DrawRequest
  {
  public:
    TextRequest() = default;

    virtual void render(Renderer& renderer) const override;

  public:
    std::string m_text;
    std::string m_font;
    int m_size;
    Vector m_pos;
    Renderer::TextAlign m_align;
  };

public:
  DrawingContext() = delete;
  DrawingContext(Renderer& renderer);

  void draw_filled_rect(const Rect& rect, const Color& color,
                        const Renderer::Blend& blend, int layer);
  void draw_texture(const Texture& texture, const Rect& srcrect,
                    const Rect& dstrect, float angle, const Color& color,
                    const Renderer::Blend& blend, int layer);
  void draw_texture(const std::shared_ptr<Texture>& texture, const Rect& srcrect,
                    const Rect& dstrect, float angle, const Color& color,
                    const Renderer::Blend& blend, int layer);
  void draw_text(const std::string& text, const Vector& pos,
                 Renderer::TextAlign align, const std::string& fontfile,
                 int size, const Color& color, const Renderer::Blend& blend,
                 int layer);
  void render(Texture* texture = nullptr) const;
  void clear();

  Renderer& get_renderer() const;

private:
  Renderer& m_renderer;
  std::map<int, std::vector<std::unique_ptr<DrawRequest>>> m_requests;

private:
  DrawingContext(const DrawingContext&) = delete;
  DrawingContext& operator=(const DrawingContext&) = delete;
};

#endif
