#pragma once

#include <whirl/rpc/impl/raw_value.hpp>
#include <whirl/rpc/impl/errors.hpp>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

// Typed method helpers

namespace detail {

template <typename PackedArgs>
PackedArgs DeserializeInput(const BytesValue& input) {
  try {
    return Deserialize<PackedArgs>(input);
  } catch (...) {
    throw RPCBadRequest();
  }
}

template <typename... Types>
struct Pack {
  using Tuple = std::tuple<typename std::decay<Types>::type...>;
};

template <typename Result, typename... Args>
struct InvokeHelper {
  template <typename M>
  static BytesValue Invoke(M f, const BytesValue& input) {
    using PackedArgs = typename Pack<Args...>::Tuple;

    auto packed_args = DeserializeInput<PackedArgs>(input);
    Result result =
        std::apply(std::move(f), std::forward<PackedArgs>(packed_args));
    return Serialize(result);
  }
};

template <typename... Args>
struct InvokeHelper<void, Args...> {
  template <typename M>
  static BytesValue Invoke(M f, const BytesValue& input) {
    using PackedArgs = typename Pack<Args...>::Tuple;

    auto packed_args = DeserializeInput<PackedArgs>(input);
    std::apply(std::move(f), std::forward<PackedArgs>(packed_args));
    return "";
  }
};

}  // namespace detail

}  // namespace whirl::rpc
