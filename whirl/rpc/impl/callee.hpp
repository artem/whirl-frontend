#pragma once

#include <string>

#include <whirl/helpers/serialize.hpp>

#include <fmt/ostream.h>

namespace whirl::rpc {

struct Callee {
  std::string service;
  std::string method;

  static Callee Parse(std::string callee);

  SERIALIZE(CEREAL_NVP(service), CEREAL_NVP(method))
};

std::ostream& operator<< (std::ostream& out, const Callee& callee);

}  // namespace whirl::rpc
