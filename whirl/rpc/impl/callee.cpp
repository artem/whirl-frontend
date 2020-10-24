#include <whirl/rpc/impl/callee.hpp>

#include <whirl/helpers/string_utils.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::rpc {

Callee Callee::Parse(std::string callee) {
  auto parts = Split(callee, '.');
  WHEELS_VERIFY(parts.size() == 2, "Invalid callee format: '" << callee << "'");
  return {parts[0], parts[1]};
}

}  // namespace whirl::rpc
