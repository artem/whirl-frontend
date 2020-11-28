#include <whirl/matrix/log/level.hpp>

namespace whirl {

std::string LogLevelToString(LogLevel level) {
  switch (level) {
    case LogLevel::Debug: return "Debug";
    case LogLevel::Info: return "Info";
    case LogLevel::Error: return "Error";
    default: return "?";
  }
}

}  // namespace whirl
