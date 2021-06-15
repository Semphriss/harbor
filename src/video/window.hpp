#ifndef _HEADER_HARBOR_VIDEO_WINDOW_HPP
#define _HEADER_HARBOR_VIDEO_WINDOW_HPP

#include <string>

class Window
{
public:
  Window() = default;
  virtual ~Window() = default;

  virtual std::string get_title() const = 0;
  virtual void set_title(const std::string& title) = 0;

private:
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
};

#endif
