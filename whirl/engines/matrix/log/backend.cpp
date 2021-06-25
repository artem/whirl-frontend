#include <whirl/engines/matrix/log/backend.hpp>

#include <whirl/engines/matrix/log/format.hpp>

#include <whirl/logger/enabled.hpp>

#include <whirl/engines/matrix/memory/new.hpp>

#include <iostream>

namespace whirl::matrix {

LogBackend::LogBackend() {
#if defined(WHIRL_LOGGING_ENABLED)
  file_ = GetLogFile();
#endif
  InitLevels();
}

void LogBackend::Write(const LogEvent& event) {
  events_.push_back(event);

#if defined(WHIRL_LOGGING_ENABLED)
  FormatLogEventTo(event, file_);
  file_ << std::endl;
#endif
}

static LogLevel kDefaultMinLogLevel = LogLevel::Info;

void LogBackend::InitLevels() {
  levels_ = GetLogLevelsFromEnv();
}

LogLevel LogBackend::GetMinLevel(const std::string& component) const {
  if (auto it = levels_.find(component); it != levels_.end()) {
    return it->second;
  }
  return kDefaultMinLogLevel;
}

void LogBackend::Log(const std::string& component, LogLevel level,
                     const std::string& message) {
  GlobalAllocatorGuard g;
  Write(MakeLogEvent(component, level, message));
}

}  // namespace whirl::matrix
