#include <whirl/matrix/log/log.hpp>

#include <whirl/matrix/log/enabled.hpp>

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

static void WriteTo(const LogEvent& event, std::ostream& out) {
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

Log::Log() {
#if defined(WHIRL_LOGGING_ENABLED)
  file_ = GetLogFile();
#endif
  InitLevels();
}

void Log::Write(const LogEvent& event) {
  std::ostringstream event_out;
  WriteTo(event, event_out);
  std::string event_str = event_out.str();

  memory_ << event_str << std::endl;

#if defined(WHIRL_LOGGING_ENABLED)
  file_ << event_str << std::endl;
#endif
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
