#pragma once

#include <whirl/rpc/impl/raw_value.hpp>
#include <whirl/rpc/impl/exceptions.hpp>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

// Typed method helpers

namespace detail {

template <typename ArgsTuple>
ArgsTuple DeserializeInput(const BytesValue& input) {
  try {
    return Deserialize<ArgsTuple>(input);
  } catch (...) {
    throw BadRequest("Arguments mismatch");
  }
}

template <typename... Types>
struct Pack {
  using Tuple = std::tuple<typename std::decay<Types>::type...>;
};

template <typename Result, typename... Args>
struct InvokeHelper {
  template <typename F>
  static BytesValue Invoke(F f, const BytesValue& input) {
    using ArgsTuple = typename Pack<Args...>::Tuple;

    auto args = DeserializeInput<ArgsTuple>(input);
    Result result =
        std::apply(std::move(f), std::forward<ArgsTuple>(args));
    return Serialize(result);
  }
};

template <typename... Args>
struct InvokeHelper<void, Args...> {
  template <typename F>
  static BytesValue Invoke(F f, const BytesValue& input) {
    using ArgsTuple = typename Pack<Args...>::Tuple;

    auto args = DeserializeInput<ArgsTuple>(input);
    std::apply(std::move(f), std::forward<ArgsTuple>(args));
    return "";
  }
};

}  // namespace detail

}  // namespace whirl::rpc
