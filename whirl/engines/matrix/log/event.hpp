#pragma once

#include <whirl/logger/level.hpp>

#include <whirl/time.hpp>

#include <string>
#include <optional>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

struct LogEvent {
  TimePoint time;
  size_t step;
  LogLevel level;
  std::string actor;
  std::string component;
  std::optional<std::string> trace_id;
  std::string message;
};

//////////////////////////////////////////////////////////////////////

// Capture event context
LogEvent MakeLogEvent(const std::string& component, LogLevel level,
                      const std::string& message);

}  // namespace whirl::matrix
