#pragma once

#include <whirl/matrix/log/event.hpp>

#include <fmt/core.h>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class Logger {
 public:
  Logger(const std::string& component);

  void Log(LogLevel level, const std::string& message);

 private:
  LogEvent MakeEvent(LogLevel level, const std::string& message) const;
  void Write(const LogEvent& event);

 private:
  std::string component_;
};

//////////////////////////////////////////////////////////////////////

#ifndef NDEBUG

// TODO: at least one argument for format string

#define WHIRL_SIM_LOG_IMPL(level, ...) logger_.Log(level, fmt::format(__VA_ARGS__))

#define WHIRL_SIM_LOG_DEBUG(...) WHIRL_SIM_LOG_IMPL(LogLevel::Debug, __VA_ARGS__)
#define WHIRL_SIM_LOG_INFO(...)  WHIRL_SIM_LOG_IMPL(LogLevel::Info, __VA_ARGS__)
#define WHIRL_SIM_LOG_ERROR(...) WHIRL_SIM_LOG_IMPL(LogLevel::Error, __VA_ARGS__)

// Backward compatibility
#define WHIRL_SIM_LOG(...) WHIRL_SIM_LOG_INFO(__VA_ARGS__)

#else

#define WHIRL_SIM_LOG_DEBUG(...)
#define WHIRL_SIM_LOG_INFO(...)
#define WHIRL_SIM_LOG_ERROR(...)

#define WHIRL_SIM_LOG(...)

#endif

}  // namespace whirl
