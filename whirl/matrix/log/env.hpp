#pragma once

#include <whirl/matrix/log/level.hpp>

#include <map>
#include <string>

namespace whirl {

// Component -> Min log level
using LogLevels = std::map<std::string, LogLevel>;

// Usage: WHIRL_LOG_LEVELS=Network=Off,Server=Off
LogLevels GetLogLevelsFromEnv();

}  // namespace whirl
