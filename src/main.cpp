#include "SDL.h"

#include "video/window.hpp"

int main()
{
  Window w;
  w.set_title("Hello, world!");

  SDL_Event e;
  bool quit = false;
  while(!quit)
  {
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
      switch(e.type)
      {
        case SDL_QUIT:
          quit = true;
          break;

        default:
          break;
      }
    }
  }

  return 0;
}
