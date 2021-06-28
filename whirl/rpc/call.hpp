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

using await::futures::Future;

template <typename T>
Future<T> As(Future<BytesValue> f_raw) {
  return std::move(f_raw).Then([](BytesValue raw) {
    return Deserialize<T>(raw);
  });
}

template <>
inline Future<void> As(Future<BytesValue> f_raw) {
  return await::futures::JustStatus(std::move(f_raw));
}

class [[nodiscard]] Caller1 {
 public:
  Caller1(Method method, BytesValue input, IChannelPtr channel)
      : method_(method),
        input_(std::move(input)),
        channel_(std::move(channel)),
        stop_token_(DefaultStopToken()) {
  }

  Caller1& StopAdvice(await::util::StopToken stop_token) {
    stop_token_ = std::move(stop_token);
    return *this;
  }

  template <typename T>
  Future<T> As() {
    return detail::As<T>(Call());
  }

  template <typename T>
  operator Future<T>() {
    return detail::As<T>(Call());
  }

 private:
  await::util::StopToken DefaultStopToken();

  CallContext MakeCallContext() {
    return {stop_token_};
  }

  Future<BytesValue> Call() {
    return channel_->Call(method_, input_, MakeCallContext());
  }

 private:
  Method method_;
  BytesValue input_;
  IChannelPtr channel_{nullptr};

  // Call context
  await::util::StopToken stop_token_;
};

class [[nodiscard]] Caller0 {
 public:
  Caller0(Method method, BytesValue input)
      : method_(method), input_(std::move(input)) {
  }

  Caller1 Via(IChannelPtr channel) {
    return Caller1(std::move(method_), std::move(input_), std::move(channel));
  }

  Method method_;
  BytesValue input_;
};

}  // namespace detail

// Unary RPC
// Usage:
// auto f = Call("EchoService.Echo", proto::Echo::Request{data})
//            .Via(channel)
//            .StopAdvice(stop_token)
//            .As<proto::Echo::Response>();
//
// .As<R>() is optional:
// Future<proto::Echo::Response> f =
//   Call("EchoService.Echo", proto::Echo::Request{data})
//      .Via(channel);

// TODO: Typestate correctness
// TODO: BlockingCall

template <typename... Arguments>
detail::Caller0 Call(const std::string& method_str, Arguments&&... arguments) {
  auto method = Method::Parse(method_str);
  // Erase argument types
  auto input = detail::SerializeInput(std::forward<Arguments>(arguments)...);
  return detail::Caller0{method, input};
}

}  // namespace whirl::rpc
