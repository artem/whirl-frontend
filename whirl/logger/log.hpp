#pragma once

#include <whirl/logger/enabled.hpp>
#include <whirl/logger/level.hpp>

#include <fmt/core.h>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Component logger

class Logger {
 public:
  Logger(const std::string& component) : component_(component) {
  }

  bool IsLevelEnabled(LogLevel level) const;
  void Log(LogLevel level, const std::string& message);

 private:
  std::string component_;
};

//////////////////////////////////////////////////////////////////////

#if defined(WHIRL_LOGGING_ENABLED)

#define WHIRL_LOG_IMPL(level, ...)                  \
  do {                                              \
    if (logger_.IsLevelEnabled(level)) {            \
      logger_.Log(level, fmt::format(__VA_ARGS__)); \
    }                                               \
  } while (false)

#else

#define WHIRL_LOG_IMPL(level, ...)

#endif

#define WHIRL_LOG_DEBUG(...) WHIRL_LOG_IMPL(LogLevel::Debug, __VA_ARGS__)
#define WHIRL_LOG_INFO(...) WHIRL_LOG_IMPL(LogLevel::Info, __VA_ARGS__)
#define WHIRL_LOG_WARN(...) WHIRL_LOG_IMPL(LogLevel::Warning, __VA_ARGS__)
#define WHIRL_LOG_ERROR(...) WHIRL_LOG_IMPL(LogLevel::Error, __VA_ARGS__)

// Backward compatibility
#define WHIRL_LOG(...) WHIRL_LOG_INFO(__VA_ARGS__)

}  // namespace whirl
