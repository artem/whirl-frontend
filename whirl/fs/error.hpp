#pragma once

#include <stdexcept>

namespace whirl::node::fs {

class FsError : public std::runtime_error {
 public:
  FsError(const std::string what) : std::runtime_error(what) {
  }
};

}  // namespace whirl::node::fs
