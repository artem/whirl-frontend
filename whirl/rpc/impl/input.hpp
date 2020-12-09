#pragma once

#include <whirl/rpc/impl/bytes_value.hpp>
#include <whirl/rpc/impl/exceptions.hpp>

#include <whirl/helpers/serialize.hpp>

#include <tuple>

namespace whirl::rpc {

namespace detail {

template<typename ... Types>
BytesValue SerializeInput(Types &&... arguments) {
  auto packed_values =
      std::make_tuple(std::forward<Types>(arguments)...);
  return Serialize(packed_values);
};

template<typename... Types>
struct Packer {
  using ValuesTuple = std::tuple<typename std::decay<Types>::type...>;
};

template<typename ... Types>
auto DeserializeInput(BytesValue input) {
  using ValuesTuple = typename Packer<Types...>::ValuesTuple;

  try {
    return Deserialize<ValuesTuple>(input);
  } catch (...) {
    throw BadRequest("Arguments mismatch");
  }
};

}  // namespace detail

}  // namespace whirl::rpc
