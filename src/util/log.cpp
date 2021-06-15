#include "util/log.hpp"

std::ostream&
Log::log(const char* level, const char* file, int line)
{
  std::cerr << "[" << level << "] " << file << ":" << line << ": ";
  return std::cerr;
}
