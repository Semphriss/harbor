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

#include "video/texture.hpp"

class Renderer;

class Window
{
public:
  enum class VideoSystem {
    SDL
  };

public:
  static std::unique_ptr<Window> create_window(VideoSystem vs);

public:
  virtual ~Window() = default;

  virtual Texture& load_texture(const std::string& file) = 0;
  virtual std::unique_ptr<Texture> create_texture(const Size& size) = 0;
  virtual Renderer& get_renderer() = 0;
  virtual std::string get_title() const = 0;
  virtual void set_title(const std::string& title) = 0;

  void flush_texture_cache();

protected:
  Window() = default;

protected:
  std::unordered_map<std::string, std::unique_ptr<Texture>> m_texture_cache;

private:
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
};

#endif
