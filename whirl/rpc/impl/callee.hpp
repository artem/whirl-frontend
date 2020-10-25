#pragma once

#include <string>

#include <whirl/helpers/serialize.hpp>

namespace whirl::rpc {

struct Callee {
  std::string service;
  std::string method;

  std::string ToString() const {
    return service + "." + method;
  }

  static Callee Parse(std::string callee);

  SERIALIZE(CEREAL_NVP(service), CEREAL_NVP(method))
};

}  // namespace whirl::rpc
