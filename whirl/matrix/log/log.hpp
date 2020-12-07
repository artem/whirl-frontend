#pragma once

#include <whirl/matrix/log/event.hpp>
#include <whirl/matrix/log/env.hpp>
#include <whirl/matrix/log/file.hpp>

#include <iostream>

namespace whirl {

class Log {
 public:
  Log();

  void SetOutput(std::ostream* out) {
    out_ = out;
  }

  LogLevel GetMinLevel(const std::string& component) const;

  void Write(const LogEvent& event);

 private:
  void InitLevels();

 private:
  LogLevels levels_;
  std::ostream* out_;
  std::ofstream file_;
};

}  // namespace whirl
