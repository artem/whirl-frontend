#pragma once

#include <whirl/services/terminal.hpp>

#include <whirl/logger/log.hpp>

#include <whirl/engines/matrix/server/stdout.hpp>

namespace whirl::matrix {

class Terminal : public ITerminal {
 public:
  Terminal(Stdout& stdout) : stdout_(stdout) {
  }

  void PrintLine(std::string_view line) override {
    WHIRL_LOG_INFO("Stdout(\"{}\")", line);
    stdout_.PrintLine(line);
  }

 private:
  Stdout& stdout_;
  Logger logger_{"Terminal"};
};

}  // namespace whirl::matrix
