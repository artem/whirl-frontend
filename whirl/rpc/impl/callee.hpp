#pragma once

#include <string>

namespace whirl::rpc {

struct Callee {
  std::string service;
  std::string method;

  std::string ToString() const {
    return service + "." + method;
  }

  static Callee Parse(std::string callee);
};

}  // namespace whirl::rpc
