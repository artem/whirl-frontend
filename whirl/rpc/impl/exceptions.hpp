#pragma once

#include <stdexcept>

namespace whirl::rpc {

struct BadRequest : std::runtime_error {
  BadRequest(const char* what) : std::runtime_error(what) {
  }
};

}  // namespace whirl::rpc
