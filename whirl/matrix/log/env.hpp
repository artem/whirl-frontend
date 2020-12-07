#pragma once

#include <whirl/matrix/log/level.hpp>

#include <map>
#include <string>
#include <optional>

namespace whirl {

// Component -> Min log level
using LogLevels = std::map<std::string, LogLevel>;

// Usage: WHIRL_LOG_LEVELS=Network=Off,Server=Off
LogLevels GetLogLevelsFromEnv();

std::optional<std::string> GetLogPathFromEnv();

}  // namespace whirl
