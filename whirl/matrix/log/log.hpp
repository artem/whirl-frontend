#pragma once

#include <whirl/matrix/log/event.hpp>
#include <whirl/matrix/log/env.hpp>
#include <whirl/matrix/log/file.hpp>

#include <iostream>
#include <sstream>

namespace whirl {

class Log {
 public:
  Log();

  void SetOutput(std::ostream* /*ignored*/) {
    // out_ = out;
  }

  LogLevel GetMinLevel(const std::string& component) const;

  void Write(const LogEvent& event);

  std::string TextLog() const {
    return memory_.str();
  }

 private:
  void InitLevels();

 private:
  LogLevels levels_;

  std::stringstream memory_;
  std::ofstream file_;
};

}  // namespace whirl
