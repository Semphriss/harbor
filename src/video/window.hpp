#include <string>

#include "SDL.h"

class Window
{
public:
  Window();
  ~Window();

  void set_title(const std::string& title);

private:
  SDL_Window* m_window;

private:
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
};
