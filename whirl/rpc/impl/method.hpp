#pragma once

#include <string>

#include <whirl/cereal/serialize.hpp>

#include <fmt/ostream.h>

namespace whirl::rpc {

struct Method {
  std::string service;
  std::string name;

  // Format: "{service}.{name}"
  static Method Parse(std::string method);

  WHIRL_SERIALIZE(service, name)
};

std::ostream& operator<<(std::ostream& out, const Method& method);

}  // namespace whirl::rpc
