#pragma once

#include <whirl/engines/matrix/log/level.hpp>

#include <whirl/time.hpp>

#include <string>
#include <optional>

namespace whirl {

struct LogEvent {
  TimePoint time;
  size_t step;
  LogLevel level;
  std::string actor;
  std::string component;
  std::optional<std::string> trace_id;
  std::string message;
};

}  // namespace whirl
