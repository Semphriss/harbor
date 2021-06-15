#include <iostream>

#define log_(LEVEL) Log::log(#LEVEL, __FILE__, __LINE__)
#define log_debug log_(DEBUG)
#define log_info  log_(INFO)
#define log_warn  log_(WARN)
#define log_error log_(ERROR)
#define log_fatal log_(FATAL)

class Log
{
public:
  static std::ostream& log(const char* level, const char* file, int line);
};
