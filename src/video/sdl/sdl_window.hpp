#ifndef _HEADER_HARBOR_VIDEO_SDL_SDLWINDOW_HPP
#define _HEADER_HARBOR_VIDEO_SDL_SDLWINDOW_HPP

#include "video/window.hpp"

#include "SDL.h"

class SDLWindow :
  public Window
{
public:
  SDLWindow();
  virtual ~SDLWindow();

  virtual std::string get_title() const override;
  virtual void set_title(const std::string& title) override;

private:
  SDL_Window* m_window;

private:
  SDLWindow(const SDLWindow&) = delete;
  SDLWindow& operator=(const SDLWindow&) = delete;
};

#endif
