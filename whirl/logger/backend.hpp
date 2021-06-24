#pragma once

#include <whirl/logger/level.hpp>

#include <string>

namespace whirl {

struct ILoggerBackend {
  virtual ~ILoggerBackend() = default;

  virtual LogLevel GetMinLevel(const std::string& component) const = 0;

  virtual void Log(const std::string& component, LogLevel level,
                   const std::string& message) = 0;
};

// TODO
void SetLoggerBackend(ILoggerBackend* backend);
ILoggerBackend* GetLoggerBackend();

}  // namespace whirl
