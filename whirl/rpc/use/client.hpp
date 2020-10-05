#pragma once

#include <whirl/rpc/impl/channel.hpp>
#include <whirl/rpc/impl/transport_channel.hpp>

#include <cereal/types/tuple.hpp>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

namespace detail {

class TRPCResult {
 public:
  explicit TRPCResult(Future<RPCBytes>&& raw_result)
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
  Future<RPCBytes> raw_result_;
};

}  // namespace detail

//////////////////////////////////////////////////////////////////////

class TRPCChannel {
 public:
  TRPCChannel(IRPCChannelPtr impl) : impl_(impl) {
  }

  // Non-copyable
  TRPCChannel(const TRPCChannel& that) = delete;
  TRPCChannel& operator=(TRPCChannel& that) = delete;

  TRPCChannel(TRPCChannel&& that) = default;

  // Usage:
  // 1) Future<std::string> f = channel.Call("Echo", data);
  // 2) auto f = channel.Call("Echo", data).As<std::string>();

  template <typename... Arguments>
  detail::TRPCResult Call(const std::string& method, Arguments&&... arguments) {
    auto packed_arguments =
        std::make_tuple(std::forward<Arguments>(arguments)...);
    auto input = Serialize(packed_arguments);
    return detail::TRPCResult{impl_->Call(method, std::move(input))};
  }

  // Represent peer
  const std::string& Peer() const {
    return impl_->Peer();
  }

  void Close() {
    impl_->Close();
  }

 private:
  IRPCChannelPtr impl_;
};

//////////////////////////////////////////////////////////////////////

class TRPCClient {
 public:
  TRPCClient() {
  }

  TRPCClient(ITransportPtr t) : transport_(t) {
  }

  TRPCChannel Dial(const std::string& peer) {
    auto impl = std::make_shared<RPCTransportChannel>(transport_, peer);
    impl->Start();
    return TRPCChannel(impl);
  }

 private:
  ITransportPtr transport_;
};

}  // namespace whirl::rpc
