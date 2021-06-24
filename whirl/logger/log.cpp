#include <whirl/logger/log.hpp>

#include <whirl/logger/backend.hpp>

#include <wheels/support/assert.hpp>

#include <iostream>

namespace whirl {

bool Logger::IsLevelEnabled(LogLevel level) const {
  return level >= GetLoggerBackend()->GetMinLevel(component_);
}

void Logger::Log(LogLevel level, const std::string& message) {
  GetLoggerBackend()->Log(component_, level, message);
}

}  // namespace whirl
