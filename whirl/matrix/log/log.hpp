#pragma once

#include <whirl/matrix/log/event.hpp>
#include <whirl/matrix/log/env.hpp>

#include <iostream>

namespace whirl {

class Log {
 public:
  Log() : out_(&std::cout) {
    InitLevels();
  }

  void SetOutput(std::ostream* out) {
    out_ = out;
  }

  LogLevel GetMinLevel(const std::string& component) const;

  void Write(const LogEvent& event) {
    WriteTo(event, *out_);
    *out_ << std::endl;
  }

 private:
  void InitLevels();
  static void WriteTo(const LogEvent& event, std::ostream& out);

 private:
  LogLevels levels_;
  std::ostream* out_;
};

}  // namespace whirl
