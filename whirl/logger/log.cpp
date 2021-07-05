#include <whirl/logger/log.hpp>

#include <whirl/logger/backend.hpp>

namespace whirl {

bool Logger::IsLevelEnabled(LogLevel level) const {
  return level >= GetLoggerBackend()->GetMinLevelFor(component_);
}

void Logger::Log(LogLevel level, const std::string& message) const {
  GetLoggerBackend()->Log(component_, level, message);
}

}  // namespace whirl
