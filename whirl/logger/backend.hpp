#pragma once

#include <whirl/logger/level.hpp>

#include <string>
#include <functional>

namespace whirl {

//////////////////////////////////////////////////////////////////////

struct ILoggerBackend {
  virtual ~ILoggerBackend() = default;

  virtual LogLevel GetMinLevelFor(const std::string& component) const = 0;

  virtual void Log(const std::string& component, LogLevel level,
                   const std::string& message) = 0;
};

//////////////////////////////////////////////////////////////////////

using LoggerBackendAccessor = std::function<ILoggerBackend*()>;

// Set at startup
void SetLoggerBackend(LoggerBackendAccessor accessor);

// Used by Logger-s
ILoggerBackend* GetLoggerBackend();

}  // namespace whirl
