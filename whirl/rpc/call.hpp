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

template <typename T>
Future<T> As(Future<BytesValue> f_raw) {
  return std::move(f_raw).Then([](BytesValue raw) {
    return Deserialize<T>(raw);
  });
}

template <>
Future<void> As(Future<BytesValue> f_raw) {
  return await::futures::JustStatus(std::move(f_raw));
}

class [[nodiscard]] Caller {
 public:
  Caller(Method method, BytesValue input)
      : method_(method), input_(std::move(input)) {
  }

  Caller& Via(IChannelPtr channel) {
    channel_ = channel;
    return *this;
  }

  // TODO: WithContext

  template <typename T>
  Future<T> As() {
    return detail::As<T>(Call());
  }

  template <typename T>
  operator Future<T>() {
    return detail::As<T>(Call());
  }

 private:
  Future<BytesValue> Call() {
    return channel_->Call(method_, input_);
  }

 private:
  Method method_;
  BytesValue input_;
  IChannelPtr channel_{nullptr};
};

}  // namespace detail

// Unary RPC
// Usage:
// 1) Future<std::string> f = Call(channel, "EchoService.Echo", data);
// 2) auto f = Call(channel, "EchoService.Echo", data).As<std::string>()

// TODO: Typestate correctness
// TODO: BlockingCall

template <typename... Arguments>
detail::Caller Call(const std::string& method_str, Arguments&&... arguments) {
  auto method = Method::Parse(method_str);
  // Erase argument types
  auto input = detail::SerializeInput(std::forward<Arguments>(arguments)...);
  return detail::Caller{method, input};
}

}  // namespace whirl::rpc
