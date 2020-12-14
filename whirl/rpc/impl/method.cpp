#include <whirl/rpc/impl/method.hpp>

#include <whirl/helpers/string_utils.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::rpc {

Method Method::Parse(std::string method) {
  auto parts = Split(method, '.');
  WHEELS_VERIFY(parts.size() == 2, "Invalid method format: '" << method << "', expected {service}.{method_name}");
  return {parts[0], parts[1]};
}

std::ostream& operator<<(std::ostream& out, const Method& method) {
  out << method.service << "." << method.name;
  return out;
}

}  // namespace whirl::rpc
