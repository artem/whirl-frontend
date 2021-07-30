#pragma once

#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

enum class LogLevel {
  All = 0,  // Do not format
  Debug = 1,
  Info = 2,
  Warning = 3,
  Error = 4,
  Off = 100500
};

//////////////////////////////////////////////////////////////////////

// LogLevel::Warning <-> "Warning"

std::string LogLevelToString(LogLevel level);
LogLevel LogLevelFromString(const std::string& level_str);

}  // namespace whirl
