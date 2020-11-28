#pragma once

#include <whirl/matrix/log/event.hpp>

#include <fmt/core.h>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class Logger {
 public:
  Logger(const std::string& component);

  void Log(const std::string& message);

 private:
  LogEvent MakeEvent(const std::string& message) const;
  void Write(const LogEvent& event);

 private:
  std::string component_;
};

//////////////////////////////////////////////////////////////////////

#ifndef NDEBUG

// TODO: at least one argument for format string
#define WHIRL_FMT_LOG(...) logger_.Log(fmt::format(__VA_ARGS__))

#else

#define WHIRL_FMT_LOG(...)

#endif

}  // namespace whirl
