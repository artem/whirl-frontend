#pragma once

#include <stdexcept>

namespace whirl {

class FsError : public std::runtime_error {
 public:
  FsError(const std::string what) : std::runtime_error(what) {
  }
};

}  // namespace whirl
