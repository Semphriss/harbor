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

#ifndef _HEADER_HARBOR_VIDEO_SDL_SDLRENDERER_HPP
#define _HEADER_HARBOR_VIDEO_SDL_SDLRENDERER_HPP

#include "video/renderer.hpp"

class SDLWindow;

class SDLRenderer final :
  public Renderer
{
public:
  SDLRenderer() = delete;
  SDLRenderer(SDLWindow& window);
  virtual ~SDLRenderer() override = default;

  virtual void draw_filled_rect(const Rect& rect, const Color& color,
                                const Blend& blend) override;
  virtual void draw_texture(const Texture& texture, const Rect& srcrect,
                            const Rect& dstrect, const Color& color,
                            const Blend& blend) override;
  virtual void draw_text(const std::string& text, const Vector& pos,
                         TextAlign align, const std::string& fontfile, int size,
                         const Color& color, const Blend& blend) override;
  virtual void start_draw(Texture* texture = nullptr) override;
  virtual void end_draw() override;

  SDL_Renderer* get_sdl_renderer() const;

private:
  SDL_Renderer* m_sdl_renderer;

private:
  SDLRenderer(const SDLRenderer&) = delete;
  SDLRenderer& operator=(const SDLRenderer&) = delete;
};

#endif
