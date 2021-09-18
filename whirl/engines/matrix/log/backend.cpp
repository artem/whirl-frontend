#include <whirl/engines/matrix/log/backend.hpp>

#include <whirl/engines/matrix/log/format.hpp>

#include <whirl/engines/matrix/new/new.hpp>

#include <iostream>

namespace whirl::matrix::log {

LogBackend::LogBackend() {
#ifndef NDEBUG
  file_ = GetLogFile();
#endif
  InitLevels();
}

void LogBackend::Write(const LogEvent& event) {
  events_.push_back(event);

#ifndef NDEBUG
  FormatLogEventTo(event, file_);
  file_ << std::endl;
#endif
}

static timber::Level kDefaultMinLogLevel = timber::Level::Info;

void LogBackend::InitLevels() {
  levels_ = GetLogLevelsFromEnv();
}

timber::Level LogBackend::GetMinLevelFor(const std::string& component) const {
  if (auto it = levels_.find(component); it != levels_.end()) {
    return it->second;
  }
  return kDefaultMinLogLevel;
}

void LogBackend::Log(timber::Event event) {
  GlobalAllocatorGuard g;
  Write(MakeLogEvent(event));
}

}  // namespace whirl::matrix::log
