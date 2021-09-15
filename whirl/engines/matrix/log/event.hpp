#pragma once

#include <timber/event.hpp>

#include <whirl/time.hpp>

#include <string>
#include <optional>
#include <vector>

namespace whirl::matrix::log {

//////////////////////////////////////////////////////////////////////

struct LogEvent {
  TimePoint time;
  size_t step;
  timber::Level level;
  std::string actor;
  std::string component;
  std::optional<std::string> trace_id;
  std::string message;
};

//////////////////////////////////////////////////////////////////////

// Capture event context
LogEvent MakeLogEvent(const timber::Event& event);

//////////////////////////////////////////////////////////////////////

using EventLog = std::vector<LogEvent>;

}  // namespace whirl::matrix::log
