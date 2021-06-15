#include "video/window.hpp"

Window::Window() :
  m_window()
{
  m_window = SDL_CreateWindow("",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              640,
                              400,
                              SDL_WINDOW_SHOWN);
}

Window::~Window()
{
  SDL_DestroyWindow(m_window);
}

void
Window::set_title(const std::string& title)
{
  SDL_SetWindowTitle(m_window, title.c_str());
}

std::string
Window::get_title() const
{
  return std::string(SDL_GetWindowTitle(m_window));
}
