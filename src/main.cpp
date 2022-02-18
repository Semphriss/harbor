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

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <fstream>

#ifdef EMSCRIPTEN
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
//#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_ttf.h"
#else
#include "SDL.h"
#include "SDL_image.h"
//#include "SDL_mixer.h"
#include "SDL_ttf.h"
#endif

#include "ui/textbox.hpp"
#include "util/color.hpp"
#include "util/log.hpp"
#include "util/rect.hpp"
#include "util/vector.hpp"
#include "video/drawing_context.hpp"
#include "video/sdl/sdl_window.hpp"

#ifndef DATA_ROOT
#define DATA_ROOT "../data"
#endif

static std::unique_ptr<Window> w = nullptr;
static Textbox g_textbox{100, Rect(10, 200, 310, 230), {}, nullptr};

extern "C"
#ifdef EMSCRIPTEN
void
#else
bool
#endif
loop()
{
  try
  {
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
      switch(e.type)
      {
        case SDL_QUIT:
#ifndef EMSCRIPTEN
          return false;
#endif
          break;

        default:
          g_textbox.event(e);
          break;
      }
    }

    auto& r = w->get_renderer();
    DrawingContext dc(r);
    auto& t = w->load_texture(DATA_ROOT "/images/missing.png");
    dc.draw_filled_rect(Rect(20, 10, 100, 300), Color(0.5f, 0.25f, 0.125f),
                        Renderer::Blend::BLEND, 5);
    dc.draw_filled_rect(Rect(10, 5, 400, 50), Color(0.5f, 0.25f, 1.f),
                        Renderer::Blend::BLEND, 1);

    Rect t_rect(Vector(), t.get_size());
    dc.draw_texture(t, t_rect, t_rect, 0.f, Color(1.f, 1.f, 1.f),
                    Renderer::Blend::BLEND, 3);

    auto canvas = w->create_texture(Size(50.f, 75.f));
    dc.render(canvas.get());

    dc.draw_texture(*canvas, Rect(Vector(), canvas->get_size()),
                    Rect(Vector(300, 100), Size(100.f, 150.f)), 25.f,
                    Color(1.f, 1.f, 1.f), Renderer::Blend::ADD, 10);
    dc.draw_text("Hello, world!", Vector(10, 10), Renderer::TextAlign::TOP_LEFT,
                 DATA_ROOT "/fonts/SuperTux-Medium.ttf", 16, Color(1.f, 1.f, 1.f),
                 Renderer::Blend::BLEND, 11);

    g_textbox.draw(dc);
    dc.render();
    dc.clear();
  }
  catch(std::exception& e)
  {
    log_error << "Uncaught loop exception: " << e.what() << std::endl;
  }
  catch(...)
  {
    log_error << "Uncaught loop error" << std::endl;
  }

#ifndef EMSCRIPTEN
  return true;
#endif
}

extern "C"
int main(int /* argc */, char** /* argv */)
{
  std::ifstream file(DATA_ROOT "/images/missing.png");
  if(!file.is_open())
  {
    std::cout << "Oops!" << std::endl;
  }
  else 
  {
    std::cout << "All g" << std::endl;
  }

  Log::s_level = Log::Level::ALL;
  log_info << "Data root: " << DATA_ROOT << std::endl;
/*
  int audio_rate = 44100;
  Uint16 audio_format = AUDIO_S16SYS;
  int audio_channels = 2;
  int audio_buffers = 4096;

  if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers))
  {
    log_fatal << "Unable to initialize audio: " << Mix_GetError() << std::endl;
    return 1;
  }

  Mix_Chunk *sound = Mix_LoadWAV(DATA_ROOT "/music/chipdisko.wav");
  if(!sound)
  {
    log_fatal << "Unable to load WAV file: " << Mix_GetError() << std::endl;
    return 1;
  }
*/
/*
  int channel = Mix_PlayChannel(-1, sound, 0);
  if (channel == -1)
  {
    log_error << "Unable to play WAV file: " << Mix_GetError() << std::endl;
  }
*/
  try
  {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    w = Window::create_window(Window::VideoSystem::SDL);
    w->set_title("Hello, world!");

    g_textbox.get_theme().active.font = DATA_ROOT "/fonts/SuperTux-Medium.ttf";
    g_textbox.get_theme().active.bg_color = Color(.8f, .8f, .8f);
    g_textbox.get_theme().active.fontsize = 16;
    g_textbox.get_theme().active.fg_blend = Renderer::Blend::BLEND;
    g_textbox.get_theme().active.top.padding = 2.f;
    g_textbox.get_theme().active.left.padding = 2.f;
    g_textbox.get_theme().active.right.padding = 2.f;
    g_textbox.get_theme().active.bottom.padding = 2.f;
    g_textbox.get_theme().disabled = g_textbox.get_theme().active;
    g_textbox.get_theme().focus = g_textbox.get_theme().active;
    g_textbox.get_theme().hover = g_textbox.get_theme().active;
    g_textbox.get_theme().normal = g_textbox.get_theme().active;

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(loop, -1, false);
#else
    while(loop())
      SDL_Delay(10);
#endif
/*
    Mix_PlayChannel(-1, nullptr, 0);
    Mix_FreeChunk(sound);
    Mix_CloseAudio();
*/
  }
  catch(std::exception& e)
  {
    log_fatal << "Uncaught exception: " << e.what() << std::endl;
  }
  catch(...)
  {
    log_fatal << "Uncaught error" << std::endl;
  }

  return 0;
}
