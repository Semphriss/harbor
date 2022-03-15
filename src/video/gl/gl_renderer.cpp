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

#include "video/gl/gl_renderer.hpp"

#include <stdexcept>

#include "video/font.hpp"
#include "video/gl/gl_texture.hpp"
#include "video/gl/gl_window.hpp"
#include "util/color.hpp"
#include "util/math.hpp"
#include "util/rect.hpp"
#include "util/vector.hpp"

GLRenderer::GLRenderer(GLWindow& window) :
  Renderer(window),
  m_glwindow(window),
  m_gl_renderer(SDL_GL_CreateContext(window.get_sdl_window())),
  m_target(-1)
{
}

GLRenderer::~GLRenderer()
{
  SDL_GL_DeleteContext(m_gl_renderer);
}

void
GLRenderer::draw_filled_rect(const Rect& rect, const Color& color,
                              const Blend& blend)
{
  if (!is_drawing())
  {
    throw std::runtime_error("Call to GLRenderer::draw_filled_rect while not "
                             "drawing");
  }

  glEnable(GL_BLEND);
  set_gl_blend(blend);

  glBegin(GL_QUADS);

  glColor4f(color.r, color.g, color.b, color.a);
  glVertex2f(rect.x1, rect.y1);
  glVertex2f(rect.x2, rect.y1);
  glVertex2f(rect.x2, rect.y2);
  glVertex2f(rect.x1, rect.y2);

  glEnd();

  glDisable(GL_BLEND);
}

void
GLRenderer::draw_texture(const Texture& texture, const Rect& srcrect,
                          const Rect& dstrect, float angle, const Color& color,
                          const Blend& blend)
{
  if (!is_drawing())
  {
    throw std::runtime_error("Call to GLRenderer::draw_texture while not "
                             "drawing");
  }

  if (&texture.get_window() != &get_window())
  {
    throw std::runtime_error("Attempt to use GLRenderer::draw_texture() "
                             "with a texture not targeting the current window");
  }

  const GLTexture& t = static_cast<const GLTexture&>(texture);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  set_gl_blend(blend);

  // TODO: Support blend mode
  glBindTexture(GL_TEXTURE_2D, t.get_gl_texture());
  glColor4f(color.r, color.g, color.b, color.a);

  glBegin(GL_QUADS);

  float w = srcrect.width() / t.get_size().w;
  float h = srcrect.height() / t.get_size().h;

  glTexCoord2f(0.f, 0.f);
  Vector tl = Math::rotate(dstrect.top_lft(), dstrect.mid(), angle);
  glVertex2f(tl.x, tl.y);

  glTexCoord2f(w, 0.f);
  Vector tr = Math::rotate(dstrect.top_rgt(), dstrect.mid(), angle);
  glVertex2f(tr.x, tr.y);

  glTexCoord2f(w, h);
  Vector br = Math::rotate(dstrect.bot_rgt(), dstrect.mid(), angle);
  glVertex2f(br.x, br.y);

  glTexCoord2f(0.f, h);
  Vector bl = Math::rotate(dstrect.bot_lft(), dstrect.mid(), angle);
  glVertex2f(bl.x, bl.y);

  glEnd();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
}

void
GLRenderer::draw_text(const std::string& text, const Vector& pos,
                       const Rect& /* clip */, TextAlign align,
                       const std::string& fontfile, int size,
                       const Color& color, const Blend& blend)
{
  if (!is_drawing())
  {
    throw std::runtime_error("Call to GLRenderer::draw_text while not "
                             "drawing");
  }

  auto& font = Font::get_font(fontfile, size);

  SDL_Surface* surface = get_font_surface(font, text);
  auto* image = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  set_gl_blend(blend);

  GLuint text_texture;
  glGenTextures(1, &text_texture);
  glBindTexture(GL_TEXTURE_2D, text_texture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // TODO: Support blend mode

  Rect dst(Vector(static_cast<int>(pos.x), static_cast<int>(pos.y)),
           Size(surface->w, surface->h));

  switch(align) {
    case TextAlign::TOP_LEFT:
      break;

    case TextAlign::TOP_MID:
      dst.move(Vector(surface->w / 2, 0.f));
      break;

    case TextAlign::TOP_RIGHT:
      dst.move(Vector(surface->w, 0.f));
      break;

    case TextAlign::MID_LEFT:
      dst.move(Vector(0.f, -surface->h / 2));
      break;

    case TextAlign::CENTER:
      dst.move(Vector(surface->w / 2, -surface->h / 2));
      break;

    case TextAlign::MID_RIGHT:
      dst.move(Vector(surface->w, -surface->h / 2));
      break;

    case TextAlign::BOTTOM_LEFT:
      dst.move(Vector(0.f, -surface->h));
      break;

    case TextAlign::BOTTOM_MID:
      dst.move(Vector(surface->w / 2, -surface->h));
      break;

    case TextAlign::BOTTOM_RIGHT:
      dst.move(Vector(surface->w, -surface->h));
      break;
  }

  glColor4f(color.r, color.g, color.b, color.a);

  glBegin(GL_QUADS);

  glTexCoord2i(0, 0);
  glVertex2f(dst.x1, dst.y1);
  glTexCoord2i(1, 0);
  glVertex2f(dst.x2, dst.y1);
  glTexCoord2i(1, 1);
  glVertex2f(dst.x2, dst.y2);
  glTexCoord2i(0, 1);
  glVertex2f(dst.x1, dst.y2);

  glEnd();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);

  SDL_FreeSurface(image);
}

void
GLRenderer::draw_line(const Vector& p1, const Vector& p2, const Color& color,
                      const Blend& blend)
{
  if (!is_drawing())
  {
    throw std::runtime_error("Call to GLRenderer::draw_filled_rect while not "
                             "drawing");
  }

  glEnable(GL_BLEND);
  set_gl_blend(blend);

  glBegin(GL_LINES);

  glColor4f(color.r, color.g, color.b, color.a);
  glVertex2f(p1.x, p1.y);
  glVertex2f(p2.x, p2.y);

  glEnd();

  glDisable(GL_BLEND);
}

void
GLRenderer::start_draw(Texture* texture)
{
  Renderer::start_draw();

  if (texture && &texture->get_window() != &get_window())
  {
    throw std::runtime_error("Attempt to call GLRenderer::start_draw() with "
                             "texture unrelated to current window");
  }

  const GLTexture* gl_texture = static_cast<GLTexture*>(texture);

  SDL_GL_MakeCurrent(m_glwindow.get_sdl_window(), m_gl_renderer);

  GLsizei w = static_cast<GLsizei>(m_glwindow.get_size().w);
  GLsizei h = static_cast<GLsizei>(m_glwindow.get_size().h);
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, m_glwindow.get_size().w, 0, m_glwindow.get_size().h, -1, 1);
  glScalef(1, texture ? 1 : -1, 1);
  glTranslatef(0, texture ? 0 : -m_glwindow.get_size().h, 0);

  if (gl_texture)
  {/*
    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gl_texture->get_gl_texture(), 0);
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);*/
    m_target = gl_texture->get_gl_texture();
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, h, w, -h, 0);
  }

  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void
GLRenderer::end_draw()
{
  Renderer::end_draw();

  if (m_target != static_cast<GLuint>(-1))
  {
    GLsizei w = static_cast<GLsizei>(m_glwindow.get_size().w);
    GLsizei h = static_cast<GLsizei>(m_glwindow.get_size().h);
    glBindTexture(GL_TEXTURE_2D, m_target);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, w, h, 0);
  }

  glFlush();

  if (m_target == static_cast<GLuint>(-1))
  {
    SDL_GL_SwapWindow(m_glwindow.get_sdl_window());
  }

  m_target = -1;
}

void
GLRenderer::set_gl_blend(const Blend& blend)
{
  switch(blend)
  {
    case Blend::ADD:
      glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
      break;

    case Blend::BLEND:
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      break;

    case Blend::MODULATE:
      glBlendFunc(GL_DST_COLOR, GL_ZERO);
      break;

    case Blend::NONE:
      glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
      break;
  }
}

bool
GLRenderer::check_gl_error() const
{
  // TODO: Check for OpenGL errors

  return false;
}
