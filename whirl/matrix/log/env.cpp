#include <whirl/matrix/log/env.hpp>

#include <whirl/helpers/string_utils.hpp>

#include <wheels/support/panic.hpp>
#include <wheels/support/assert.hpp>

#include <cstdlib>

namespace whirl {

static const char* kLevelsEnvVar = "WHIRL_LOG_LEVELS";

static LogLevels ParseLogLevels(const std::string& levels_str) {
  // std::cout << "Log levels: " << levels_str << std::endl;

  LogLevels levels;

  auto components = Split(levels_str, ',');
  for (auto component_level : components) {
    auto parts = Split(component_level, '=');
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


static std::optional<std::string> GetEnvVar(const char* name) {
  char* value = std::getenv(name);
  if (value) {
    return std::string(value);
  } else {
    return std::nullopt;
  }
}

std::optional<std::string> GetLogPathFromEnv() {
  return GetEnvVar("WHIRL_LOG_FILE");
}

std::optional<std::string> GetUser() {
  return GetEnvVar("USER");
}

}  // namespace whirl
