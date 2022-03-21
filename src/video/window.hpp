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

#ifndef _HEADER_HARBOR_VIDEO_WINDOW_HPP
#define _HEADER_HARBOR_VIDEO_WINDOW_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "util/size.hpp"
#include "video/texture.hpp"

class Renderer;

class Window
{
public:
  enum class Status {
    NORMAL,
    MINIMIZED,
    MAXIMIZED,
    FULLSCREEN
  };

  enum class VideoSystem {
#if HARBOR_USE_VIDEO_SDL
    SDL,
#endif
#if HARBOR_USE_VIDEO_OPENGL
    GL,
#endif
  };

public:
  static std::unique_ptr<Window> create_window(VideoSystem vs);
  static std::string get_video_system_tag(VideoSystem vs);

public:
  Window();
  virtual ~Window() = default;

  virtual VideoSystem get_type() const = 0;

  virtual Texture& load_texture(const std::string& file) = 0;
  virtual std::shared_ptr<Texture> create_texture(const Size& size) = 0;
  virtual Renderer& get_renderer() = 0;

  virtual std::string get_title() const = 0;
  virtual Size get_size() const = 0;
  virtual bool get_visible() const = 0;
  virtual Vector get_pos() const = 0;
  virtual bool get_bordered() const = 0;
  virtual bool get_resizable() const = 0;
  virtual Status get_status() const = 0;
  virtual float get_opacity() const = 0;
  virtual std::string get_icon() const = 0;

  virtual void set_size(const Size& size) = 0;
  virtual void set_title(const std::string& title) = 0;
  virtual void set_visible(bool visible) = 0;
  virtual void set_pos(Vector pos) = 0;
  virtual void set_bordered(bool bordered) = 0;
  virtual void set_resizable(bool resizable) = 0;
  virtual void set_status(Status status) = 0;
  virtual void set_icon(const std::string& file) = 0;
  virtual void set_opacity(float opacity) = 0;

  void flush_texture_cache();
  void toggle_fullscreen();

protected:
  std::unordered_map<std::string, std::unique_ptr<Texture>> m_texture_cache;
  Status m_status_before_fullscreen;

private:
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
};

#endif
