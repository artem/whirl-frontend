#pragma once

#include <timber/level.hpp>

#include <map>
#include <string>
#include <optional>

namespace whirl::matrix {

// Component -> Min log level
using LogLevels = std::map<std::string, timber::Level>;

// Usage: WHIRL_LOG_LEVELS=Network=Off,Server=Off
LogLevels GetLogLevelsFromEnv();

// Usage: WHIRL_LOG_FILE=/path/to/log/file
std::optional<std::string> GetLogPathFromEnv();

}  // namespace whirl::matrix
