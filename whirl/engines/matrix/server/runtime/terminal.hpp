#pragma once

#include <whirl/services/terminal.hpp>

#include <whirl/engines/matrix/server/stdout.hpp>

#include <whirl/engines/matrix/world/global/log.hpp>

#include <timber/log.hpp>

namespace whirl::matrix {

class Terminal : public ITerminal {
 public:
  Terminal(Stdout& stdout) : stdout_(stdout),
        logger_("Terminal", GetLogBackend()) {
  }

  void PrintLine(std::string_view line) override {
    LOG_INFO("Stdout(\"{}\")", line);
    stdout_.PrintLine(line);
  }

 private:
  Stdout& stdout_;
  timber::Logger logger_;
};

}  // namespace whirl::matrix
