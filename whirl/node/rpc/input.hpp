#pragma once

#include <whirl/node/rpc/bytes_value.hpp>
#include <whirl/node/rpc/exceptions.hpp>

#include <whirl/cereal/tuple.hpp>

namespace whirl::rpc {

namespace detail {

// Encode RPC arguments as std::tuple

template <typename... Types>
BytesValue SerializeInput(Types&&... arguments) {
  return SerializeValues(std::forward<Types>(arguments)...);
};

template <typename... Types>
auto DeserializeInput(const BytesValue& input) {
  try {
    return DeserializeValues<Types...>(input);
  } catch (...) {
    throw BadRequest("Arguments mismatch");
  }
};

}  // namespace detail

}  // namespace whirl::rpc
