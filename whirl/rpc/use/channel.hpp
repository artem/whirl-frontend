#pragma once

#include <whirl/rpc/impl/channel.hpp>
#include <whirl/rpc/impl/input.hpp>

#include <await/futures/helpers.hpp>

#include <cereal/types/tuple.hpp>

namespace whirl::rpc {

// T stands for Typed

//////////////////////////////////////////////////////////////////////

namespace detail {

class TCallResult {
 public:
  explicit TCallResult(Future<BytesValue>&& raw_result)
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

//////////////////////////////////////////////////////////////////////

// Usage:
// 1) Future<std::string> f = channel.Call("Echo.Echo", data);
// 2) auto f = channel.Call("Echo.Echo", data).As<std::string>();

class TChannel {
 public:
  TChannel(IChannelPtr impl = nullptr) : impl_(impl) {
  }

  template <typename... Arguments>
  detail::TCallResult Call(const std::string& method_str,
                           Arguments&&... arguments) {
    auto method = Method::Parse(method_str);
    auto input = detail::SerializeInput(std::forward<Arguments>(arguments)...);
    return detail::TCallResult{impl_->Call(method, input)};
  }

  // Represent peer
  const std::string& Peer() const {
    return impl_->Peer();
  }

  void Close() {
    impl_->Close();
  }

 private:
  IChannelPtr impl_;
};

}  // namespace whirl::rpc
