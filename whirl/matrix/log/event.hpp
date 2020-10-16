#pragma once

#include <whirl/time.hpp>

#include <string>
#include <optional>

namespace whirl {

struct LogEvent {
  TimePoint time;
  size_t step;
  std::string actor;
  std::string component;
  std::optional<std::string> trace_id;
  std::string message;
};

}  // namespace whirl
