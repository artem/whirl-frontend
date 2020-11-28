#pragma once

#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

enum class LogLevel {
  All = 0,  // Do not format
  Debug = 1,
  Info = 2,
  Error = 3,
  Off = 100500
};

//////////////////////////////////////////////////////////////////////

std::string LogLevelToString(LogLevel level);

}  // namespace whirl
