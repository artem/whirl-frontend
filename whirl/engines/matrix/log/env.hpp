#pragma once

#include <whirl/logger/level.hpp>

#include <map>
#include <string>
#include <optional>

namespace whirl {

// Component -> Min log level
using LogLevels = std::map<std::string, LogLevel>;

// Usage: WHIRL_LOG_LEVELS=Network=Off,Server=Off
LogLevels GetLogLevelsFromEnv();

// Usage: WHIRL_LOG_FILE=/path/to/log/file
std::optional<std::string> GetLogPathFromEnv();

}  // namespace whirl
