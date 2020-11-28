#include <whirl/matrix/log/log.hpp>

#include <whirl/helpers/string_utils.hpp>

#include <wheels/support/assert.hpp>

#include <iostream>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Formatting

static std::string ToWidth(const std::string& s, size_t width) {
  if (s.length() > width) {
    return s.substr(0, width);
  }
  return s + std::string(width - s.length(), ' ');
}

//////////////////////////////////////////////////////////////////////

void Log::WriteTo(const LogEvent& event, std::ostream& out) {
  out << "[T " << event.time << " | " << event.step << "]"
      << "\t"
      << "[" << ToWidth(LogLevelToString(event.level), 7) << "]"
      << "\t"
      << "[" << ToWidth(event.actor, 15) << "]"
      << "\t"
      << "[" << ToWidth(event.component, 12) << "]";

  if (event.trace_id.has_value()) {
    out << "\t"
        << "[" << ToWidth(event.trace_id.value(), 6) << "]";
  }

  out << "\t" << event.message;
}

//////////////////////////////////////////////////////////////////////

static LogLevel kDefaultMinLogLevel = LogLevel::Info;

void Log::InitLevels() {
  levels_ = GetLogLevelsFromEnv();
}

LogLevel Log::GetMinLevel(const std::string& component) const {
  if (auto it = levels_.find(component); it != levels_.end()) {
    return it->second;
  }
  return kDefaultMinLogLevel;
}

}  // namespace whirl
