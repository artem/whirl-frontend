#pragma once

namespace whirl {

struct ITerminal {
  virtual ~ITerminal() = default;

  virtual void PrintLine(std::string_view line) = 0;
};

}  // namespace whirl
