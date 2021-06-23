#include <whirl/engines/matrix/log/env.hpp>

#include <wheels/support/panic.hpp>
#include <wheels/support/assert.hpp>
#include <wheels/support/env.hpp>
#include <wheels/support/string_utils.hpp>

#include <cstdlib>

namespace whirl {

static const char* kLevelsEnvVar = "WHIRL_LOG_LEVELS";

static LogLevels ParseLogLevels(const std::string& levels_str) {
  // std::cout << "Log levels: " << levels_str << std::endl;

  LogLevels levels;

  auto components = wheels::Split(levels_str, ',');
  for (auto component_level : components) {
    auto parts = wheels::Split(component_level, '=');
    WHEELS_VERIFY(parts.size() == 2, "Invalid " << kLevelsEnvVar << " format");
    std::string component = parts[0];
    std::string level = parts[1];

    levels.emplace(component, ParseLogLevel(level));
  }
  return levels;
}

LogLevels GetLogLevelsFromEnv() {
  char* levels_env = getenv(kLevelsEnvVar);
  if (!levels_env) {
    // std::cout << "No log levels..." << std::endl;
    return {};
  }

  return ParseLogLevels(levels_env);
}

std::optional<std::string> GetLogPathFromEnv() {
  return wheels::GetEnvVar("WHIRL_LOG_FILE");
}

}  // namespace whirl
