#pragma once

#include <string>

namespace whirl::node {

struct ITerminal {
  virtual ~ITerminal() = default;

  virtual void PrintLine(std::string_view line) = 0;
};

}  // namespace whirl::node
