#include <whirl/logger/level.hpp>

#include <wheels/support/panic.hpp>
#include <wheels/support/preprocessor.hpp>
#include <wheels/support/compiler.hpp>

namespace whirl {

std::string LogLevelToString(LogLevel level) {
#define LEVEL_TO_STRING_CASE(level) \
  case LogLevel::level: return TO_STRING(level)

  switch (level) {
    LEVEL_TO_STRING_CASE(Debug);
    LEVEL_TO_STRING_CASE(Info);
    LEVEL_TO_STRING_CASE(Warning);
    LEVEL_TO_STRING_CASE(Error);
    default: return "?";
  }

#undef LEVEL_TO_STRING_CASE
}

LogLevel LogLevelFromString(const std::string& level_str) {
#define FROM_STRING_IF(level)          \
  if (level_str == TO_STRING(level)) { \
    return LogLevel::level;            \
  }

  FROM_STRING_IF(Debug);
  FROM_STRING_IF(Info);
  FROM_STRING_IF(Warning);
  FROM_STRING_IF(Error);
  FROM_STRING_IF(All);
  FROM_STRING_IF(Off);

#undef FROM_STRING_IF

  WHEELS_PANIC("Unknown log level: " << level_str);

  WHEELS_UNREACHABLE();
}

}  // namespace whirl
