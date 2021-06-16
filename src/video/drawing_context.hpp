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

public:
  DrawingContext() = delete;
  DrawingContext(Renderer& renderer);

  void draw_filled_rect(const Rect& rect, const Color& color,
                        const Renderer::Blend& blend, int layer);
  void render() const;
  void clear();

private:
  Renderer& m_renderer;
  std::map<int, std::vector<std::unique_ptr<DrawRequest>>> m_requests;

private:
  DrawingContext(const DrawingContext&) = delete;
  DrawingContext& operator=(const DrawingContext&) = delete;
};

#endif
