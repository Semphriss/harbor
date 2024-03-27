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

#include "gtest/gtest.h"

#include <sstream>

#include "video/drawing_context.hpp"
#include "video/sdl/sdl_window.hpp"

#if 0
static SDLWindow w;

class MockRenderer :
  public Renderer
{
public:
  MockRenderer() :
    Renderer(w)
  {
  }

  virtual ~MockRenderer() = default;

  virtual void draw_filled_rect(const Rect& rect, const Color& color,
                                const Blend& blend) override
  {
    m_log << "draw_filled_rect(" << rect << ", " << color << ", "
          << static_cast<int>(blend) << ");\n";
  }

  virtual void draw_texture(const Texture& /* texture */, const Rect& srcrect,
                            const Rect& dstrect, float angle,
                            const Color& color, const Blend& blend) override
  {
    m_log << "draw_texture(..., " << srcrect << ", " << dstrect << ", "
          << angle << ", " << color << ", " << static_cast<int>(blend)
          << ");\n";
  }

  virtual void draw_text(const std::string& text, const Vector& pos,
                         const Rect& clip, TextAlign align,
                         const std::string& fontfile, int size,
                         const Color& color, const Blend& blend) override
  {
    m_log << "draw_text(" << text << ", " << pos << ", " << clip << ", "
          << static_cast<int>(align) << ", " << fontfile << ", " << size
          << ", " << color << ", " << static_cast<int>(blend) << ");\n";
  }

  virtual void draw_line(const Vector& p1, const Vector& p2, const Color& color,
                         const Blend& blend) override
  {
    m_log << "draw_text(" << p1 << ", " << p2 << ", " << color << ", "
          << static_cast<int>(blend) << ");\n";
  }

  virtual void start_draw(Texture* texture = nullptr)
  {
    m_log << "start_draw(" << (texture ? "..." : "nullptr") << ");\n";
  }

  virtual void end_draw()
  {
    m_log << "end_draw();\n";
  }

public:
  std::stringstream m_log;

private:
  MockRenderer(const MockRenderer&) = delete;
  MockRenderer& operator=(const MockRenderer&) = delete;
};

TEST(Video_DrawingContext, clip_src_rect)
{
  {
    Rect src(0, 0, 100, 100), dst(0, 0, 100, 100), clp(25, 25, 75, 75), res(25, 25, 75, 75);
    EXPECT_EQ(DrawingContext::clip_src_rect(src, dst, clp), res);
  }

  {
    Rect src(50, 50, 100, 100), dst(0, 0, 100, 100), clp(0, 0, 100, 100), res(50, 50, 100, 100);
    EXPECT_EQ(DrawingContext::clip_src_rect(src, dst, clp), res);
  }

  {
    Rect src(0, 0, 100, 100), dst(50, 50, 150, 150), clp(25, 25, 75, 75), res(0, 0, 25, 25);
    EXPECT_EQ(DrawingContext::clip_src_rect(src, dst, clp), res);
  }

  {
    Rect src(76, 23, 98, 80), dst(25, 25, 60, 100), clp(0, 0, 60, 250), res(76, 23, 98, 80);
    EXPECT_EQ(DrawingContext::clip_src_rect(src, dst, clp), res);
  }

  {
    Rect src(50, 50, 100, 100), dst(-60, 25, -25, 100), clp(0, 0, 10, 10), res(0, 0, 0, 0);
    EXPECT_EQ(DrawingContext::clip_src_rect(src, dst, clp), res);
  }

  {
    Rect src(76, 23, 98, 80), dst(0, 0, 100, 100), clp(0, 0, 100, 50), res(76, 23, 98, 51.5);
    EXPECT_EQ(DrawingContext::clip_src_rect(src, dst, clp), res);
  }
}

TEST(Video_DrawingContext, ctor_dtor)
{
  MockRenderer r;
  DrawingContext dc(r);
}

TEST(DrawingContext, _stress_test)
{
  MockRenderer r;
  DrawingContext dc(r);

  dc.get_transform().move(Vector(10, 10));
  dc.get_transform().scale(Size(0.5, 0.5));
  dc.get_transform().clip(Rect(20, 20, 200, 200));
  dc.get_transform().move(Vector(30, 30));
  dc.get_transform().scale(Size(4, 4));

  dc.draw_filled_rect(Rect(-10, -2, 100, 4), Color(1, 1, 1),
                      Renderer::Blend::BLEND, 1);

  dc.render();

  EXPECT_EQ(r.m_log.str(), "start_draw(nullptr);\ndraw_filled_rect(Rect(20, 23,"
                           " 110, 35), Color(1, 1, 1, 1), 1);\nend_draw();\n");
}
#endif
