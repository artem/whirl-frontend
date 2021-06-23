#include <whirl/engines/matrix/log/level.hpp>

#include <wheels/support/panic.hpp>
#include <wheels/support/preprocessor.hpp>
#include <wheels/support/compiler.hpp>

namespace whirl {

#define LEVEL_TO_STRING_CASE(level) \
  case LogLevel::level: return TO_STRING(level)

std::string LogLevelToString(LogLevel level) {
  switch (level) {
    LEVEL_TO_STRING_CASE(Debug);
    LEVEL_TO_STRING_CASE(Info);
    LEVEL_TO_STRING_CASE(Warning);
    LEVEL_TO_STRING_CASE(Error);
    default: return "?";
  }
}

#define PARSE_LEVEL(level)             \
  if (level_str == TO_STRING(level)) { \
    return LogLevel::level;            \
  }

LogLevel ParseLogLevel(const std::string& level_str) {
  PARSE_LEVEL(Debug);
  PARSE_LEVEL(Info);
  PARSE_LEVEL(Warning);
  PARSE_LEVEL(Error);
  PARSE_LEVEL(All);
  PARSE_LEVEL(Off);

  WHEELS_PANIC("Unknown log level: " << level_str);

  WHEELS_UNREACHABLE();
}

}  // namespace whirl
