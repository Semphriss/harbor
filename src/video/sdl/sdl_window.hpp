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

#ifndef _HEADER_HARBOR_VIDEO_SDL_SDLWINDOW_HPP
#define _HEADER_HARBOR_VIDEO_SDL_SDLWINDOW_HPP

#include "video/window.hpp"

#include "SDL.h"

#include "video/sdl/sdl_renderer.hpp"

class SDLWindow final :
  public Window
{
public:
  SDLWindow(const Size& size = Size(640, 400), bool visible = true);
  virtual ~SDLWindow();

  virtual Texture& load_texture(const std::string& file) override;
  virtual std::shared_ptr<Texture> create_texture(const Size& size) override;
  virtual Renderer& get_renderer() override;

  virtual std::string get_title() const override;
  virtual Size get_size() const override;
  virtual bool get_visible() const override;
  virtual Vector get_pos() const override;
  virtual bool get_bordered() const override;
  virtual bool get_resizable() const override;
  virtual Status get_status() const override;
  virtual float get_opacity() const override;
  virtual std::string get_icon() const override;

  virtual void set_size(const Size& size) override;
  virtual void set_title(const std::string& title) override;
  virtual void set_visible(bool visible) override;
  virtual void set_pos(Vector pos) override;
  virtual void set_bordered(bool bordered) override;
  virtual void set_resizable(bool resizable) override;
  virtual void set_status(Status status) override;
  virtual void set_icon(const std::string& file) override;
  virtual void set_opacity(float opacity) override;

  SDL_Window* get_sdl_window() const;
  SDLRenderer& get_sdlrenderer();

private:
  SDL_Window* m_sdl_window;
  SDLRenderer m_renderer;
  std::string m_icon_path;

private:
  SDLWindow(const SDLWindow&) = delete;
  SDLWindow& operator=(const SDLWindow&) = delete;
};

#endif
