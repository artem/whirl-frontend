#pragma once

#include <whirl/rpc/input.hpp>

#include <whirl/cereal/serialize.hpp>

#include <tuple>

namespace whirl::rpc {

// Typed method helpers

namespace detail {

template <typename Result, typename... Args>
struct InvokeHelper {
  template <typename F>
  static BytesValue Invoke(F f, const BytesValue& input) {
    auto args = DeserializeInput<Args...>(input);
    Result result = std::apply(std::move(f), std::move(args));
    return Serialize(result);
  }
};

template <typename... Args>
struct InvokeHelper<void, Args...> {
  template <typename F>
  static BytesValue Invoke(F f, const BytesValue& input) {
    auto args = DeserializeInput<Args...>(input);
    std::apply(std::move(f), std::move(args));
    return "";
  }
};

}  // namespace detail

}  // namespace whirl::rpc
