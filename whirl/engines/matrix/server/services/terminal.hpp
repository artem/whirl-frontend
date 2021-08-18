#pragma once

#include <whirl/services/terminal.hpp>

#include <whirl/engines/matrix/server/stdout.hpp>

namespace whirl::matrix {

class Terminal : public ITerminal {
 public:
  Terminal(Stdout& stdout) : stdout_(stdout) {
  }

  void PrintLine(std::string_view line) override {
    stdout_.PrintLine(line);
  }
 private:
  Stdout& stdout_;
};

}  // namespace whirl::matrix
