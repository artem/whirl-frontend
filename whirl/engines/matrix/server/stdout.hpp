#pragma once

#include <whirl/engines/matrix/memory/new.hpp>

namespace whirl::matrix {

struct Stdout {
  std::vector<std::string> lines;

  void PrintLine(std::string_view line) {
    GlobalAllocatorGuard g;
    lines.emplace_back(line);
  }
};

}  // namespace whirl::matrix