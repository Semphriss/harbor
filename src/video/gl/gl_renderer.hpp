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

#ifndef _HEADER_HARBOR_VIDEO_GL_GLRENDERER_HPP
#define _HEADER_HARBOR_VIDEO_GL_GLRENDERER_HPP

#include "video/renderer.hpp"

#ifdef EMSCRIPTEN
#include "SDL2/SDL_opengl.h"
#else
#include "SDL_opengl.h"
#endif

class GLWindow;

class GLRenderer final :
  public Renderer
{
public:
  GLRenderer() = delete;
  GLRenderer(GLWindow& window);
  virtual ~GLRenderer() override;

  virtual void draw_filled_rect(const Rect& rect, const Color& color,
                                const Blend& blend) override;
  virtual void draw_texture(const Texture& texture, const Rect& srcrect,
                            const Rect& dstrect, float angle,
                            const Color& color, const Blend& blend) override;
  virtual void draw_text(const std::string& text, const Vector& pos,
                         const Rect& clip, TextAlign align,
                         const std::string& fontfile, int size,
                         const Color& color, const Blend& blend) override;
  virtual void draw_line(const Vector& p1, const Vector& p2, const Color& color,
                         const Blend& blend) override;
  virtual void start_draw(Texture* texture = nullptr) override;
  virtual void end_draw() override;

private:
  void set_gl_blend(const Blend& blend);
  bool check_gl_error() const;

private:
  GLWindow& m_glwindow;
  SDL_GLContext m_gl_renderer;
  GLuint m_target;

private:
  GLRenderer(const GLRenderer&) = delete;
  GLRenderer& operator=(const GLRenderer&) = delete;
};

#endif
