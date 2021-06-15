#include "video/sdl/sdl_window.hpp"

SDLWindow::SDLWindow() :
  m_window()
{
  m_window = SDL_CreateWindow("",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              640,
                              400,
                              SDL_WINDOW_SHOWN);
}

SDLWindow::~SDLWindow()
{
  SDL_DestroyWindow(m_window);
}

void
SDLWindow::set_title(const std::string& title)
{
  SDL_SetWindowTitle(m_window, title.c_str());
}

std::string
SDLWindow::get_title() const
{
  return std::string(SDL_GetWindowTitle(m_window));
}
