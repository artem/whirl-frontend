#pragma once

#include <whirl/logger/backend.hpp>

#include <whirl/engines/matrix/log/event.hpp>
#include <whirl/engines/matrix/log/env.hpp>
#include <whirl/engines/matrix/log/file.hpp>

#include <iostream>
#include <sstream>
#include <vector>

namespace whirl::matrix {

class LogBackend : public ILoggerBackend {
 public:
  LogBackend();

  // Context: Server
  LogLevel GetMinLevelFor(const std::string& component) const override;

  // Context: Server
  void Log(const std::string& component, LogLevel level,
           const std::string& message) override;

  const EventLog& GetEvents() const {
    return events_;
  }

 private:
  void Write(const LogEvent& event);

  void InitLevels();

 private:
  LogLevels levels_;

  EventLog events_;
  std::ofstream file_;
};

}  // namespace whirl::matrix
