#pragma once

#include <whirl/matrix/log/event.hpp>

#include <iostream>

namespace whirl {

class Log {
 public:
  Log() : out_(&std::cout) {
  }

  void SetOutput(std::ostream* out) {
    out_ = out;
  }

  void Write(const LogEvent& event) {
    WriteTo(event, *out_);
    *out_ << std::endl;
  }

 private:
  static void WriteTo(const LogEvent& event, std::ostream& out);

 private:
  std::ostream* out_;
};

}  // namespace whirl
