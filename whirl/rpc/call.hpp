#pragma once

#include <whirl/rpc/channel.hpp>
#include <whirl/rpc/bytes_value.hpp>
#include <whirl/rpc/method.hpp>
#include <whirl/rpc/input.hpp>

#include <whirl/cereal/serialize.hpp>

#include <await/futures/helpers.hpp>

#include <cereal/types/string.hpp>

#include <optional>

namespace whirl::rpc {

namespace detail {

// Pipeline:
// --Call--> ArgsCaller
// --Args--> ViaCaller
// --Via--> Caller
// [-->StopToken --> Caller]
// --Start--> CallResult
// --As-> Future<T>

class [[nodiscard]] CallResult {
  template <typename T>
  using Future = await::futures::Future<T>;

 public:
  CallResult(Future<BytesValue> f) : raw_future_(std::move(f)) {
  }

  template <typename T>
  Future<T> As()&& {
    return std::move(raw_future_).Then([](BytesValue raw) {
      return Deserialize<T>(raw);
    });
  }

  template <>
  Future<void> As()&& {
    return await::futures::JustStatus(std::move(raw_future_));
  }

  // Implicit cast
  template <typename T>
  operator Future<T>()&& {
    return std::move(*this).As<T>();
  }

 private:
  Future<BytesValue> raw_future_;
};

class [[nodiscard]] Caller {
 public:
  Caller(Method method, BytesValue input, IChannelPtr channel)
      : method_(method),
        input_(std::move(input)),
        channel_(std::move(channel)),
        stop_token_(DefaultStopToken()) {
  }

  Caller& StopToken(await::StopToken stop_token)&& {
    stop_token_ = std::move(stop_token);
    return *this;
  }

  Caller& TraceWith(TraceId trace_id)&& {
    trace_id_ = trace_id;
    return *this;
  }

  Caller& AtMostOnce()&& {
    attempts_limit_ = 1;
    return *this;
  }

  Caller& AtLeastOnce()&& {
    attempts_limit_ = 0;
    return *this;
  }

  Caller& LimitAttempts(size_t limit)&& {
    attempts_limit_ = limit;
    return *this;
  }

  CallResult Start()&& {
    return {Call()};
  }

  template <typename T>
  operator await::futures::Future<T>() {
    return std::move(*this).Start().As<T>();
  }

 private:
  await::StopToken DefaultStopToken();
  TraceId GetTraceId();

  CallOptions MakeCallOptions() {
    return {GetTraceId(), stop_token_, attempts_limit_};
  }

  await::futures::Future<BytesValue> Call() {
    return channel_->Call(method_, input_, MakeCallOptions());
  }

 private:
  Method method_;
  BytesValue input_;
  IChannelPtr channel_{nullptr};

  // Call options
  std::optional<TraceId> trace_id_;
  await::StopToken stop_token_;
  size_t attempts_limit_{0};  // 0 == Infinite
};

class [[nodiscard]] ViaCaller {
 public:
  ViaCaller(Method method, BytesValue input)
      : method_(method), input_(std::move(input)) {
  }

  Caller Via(IChannelPtr channel)&& {
    return Caller(std::move(method_), std::move(input_), std::move(channel));
  }

 private:
  Method method_;
  BytesValue input_;
};

class [[nodiscard]] ArgsCaller {
 public:
  ArgsCaller(Method method) : method_(method) {
  }

  template <typename... TArguments>
  ViaCaller Args(TArguments... arguments) {
    auto input = detail::SerializeInput(std::forward<TArguments>(arguments)...);
    return ViaCaller{method_, input};
  }

 private:
  Method method_;
};

}  // namespace detail

// Unary RPC
// Usage:
// auto f = Call("EchoService.Echo")
//            .Args(proto::Echo::Request{data})
//            .Via(channel)
//            .StopToken(stop_token)
//            .TraceWith(trace_id)
//            .AtLeastOnce() or .AtMostOnce() or .LimitAttempts(3)
//            .Start()
//            .As<proto::Echo::Response>();
//
// .Start().As<ResponseType>() is optional:
// Future<proto::Echo::Response> f =
//   Call("EchoService.Echo", proto::Echo::Request{data})
//      .Via(channel);

inline detail::ArgsCaller Call(const std::string& method_str) {
  auto method = Method::Parse(method_str);
  return detail::ArgsCaller{method};
}

}  // namespace whirl::rpc
