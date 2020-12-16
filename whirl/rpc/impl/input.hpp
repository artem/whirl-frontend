#pragma once

#include <whirl/rpc/impl/bytes_value.hpp>
#include <whirl/rpc/impl/exceptions.hpp>

#include <whirl/cereal/tuple.hpp>

#include <tuple>

namespace whirl::rpc {

namespace detail {

template<typename ... Types>
BytesValue SerializeInput(Types &&... arguments) {
  return SerializeValues(std::forward<Types>(arguments)...);
};

template<typename ... Types>
auto DeserializeInput(const BytesValue& input) {
  try {
    return DeserializeValues<Types...>(input);
  } catch (...) {
    throw BadRequest("Arguments mismatch");
  }
};

}  // namespace detail

}  // namespace whirl::rpc
