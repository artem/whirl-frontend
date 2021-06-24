#pragma once

#include <whirl/logger/backend.hpp>

#include <whirl/engines/matrix/log/event.hpp>
#include <whirl/engines/matrix/log/env.hpp>
#include <whirl/engines/matrix/log/file.hpp>

#include <iostream>
#include <sstream>

namespace whirl {

class LogBackend : public ILoggerBackend {
 public:
  LogBackend();

  void SetOutput(std::ostream* /*ignored*/) {
    // out_ = out;
  }

  // Context: Server
  LogLevel GetMinLevel(const std::string& component) const;

  // Context: Server
  void Log(const std::string& component, LogLevel level,
           const std::string& message);

  std::string TextLog() const {
    return memory_.str();
  }

 private:
  void Write(const LogEvent& event);
  LogEvent MakeEvent(const std::string& component, LogLevel level,
                     const std::string& message) const;

  void InitLevels();

 private:
  LogLevels levels_;

  std::stringstream memory_;
  std::ofstream file_;
};

}  // namespace whirl
