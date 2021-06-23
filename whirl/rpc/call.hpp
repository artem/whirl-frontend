#pragma once

#include <whirl/rpc/channel.hpp>
#include <whirl/rpc/bytes_value.hpp>
#include <whirl/rpc/method.hpp>
#include <whirl/rpc/input.hpp>

#include <whirl/cereal/serialize.hpp>

#include <await/futures/helpers.hpp>

#include <cereal/types/string.hpp>

namespace whirl::rpc {

namespace detail {

class CallResult {
 public:
  explicit CallResult(Future<BytesValue>&& raw_result)
      : raw_result_(std::move(raw_result)) {
  }

  template <typename T>
  Future<T> As() && {
    return std::move(raw_result_).Then([](const std::string& raw) {
      return Deserialize<T>(raw);
    });
  }

  template <>
  Future<void> As<void>() && {
    return await::futures::JustStatus(std::move(raw_result_));
  }

  template <typename T>
  operator Future<T>() && {
    return std::move(*this).As<T>();
  }

 private:
  Future<BytesValue> raw_result_;
};

}  // namespace detail

// Unary RPC
// Usage:
// 1) Future<std::string> f = Call(channel, "EchoService.Echo", data);
// 2) auto f = Call(channel, "EchoService.Echo", data).As<std::string>()

template <typename... Arguments>
detail::CallResult Call(const IChannelPtr& channel,
                        const std::string& method_str,
                        Arguments&&... arguments) {
  auto method = Method::Parse(method_str);
  auto input = detail::SerializeInput(std::forward<Arguments>(arguments)...);
  return detail::CallResult{channel->Call(method, input)};
}

}  // namespace whirl::rpc
