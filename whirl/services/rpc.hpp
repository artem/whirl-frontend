#pragma once

#include <whirl/services/rpc_impl.hpp>

#include <whirl/helpers/serialize.hpp>

#include <await/futures/promise.hpp>
#include <await/futures/helpers.hpp>

#include <cereal/types/tuple.hpp>

#include <wheels/support/function_meta.hpp>

#include <functional>
#include <tuple>

namespace whirl {

using wheels::Result;

// And here goes types!

//////////////////////////////////////////////////////////////////////

// Typed method helpers

namespace detail {

template <typename Result, typename PackedArguments>
struct TypedMethod {
  template <typename M>
  static RPCBytes Invoke(M f, const RPCBytes& input) {
    auto packed_arguments = Deserialize<PackedArguments>(input);
    Result result = std::apply(std::move(f),
                               std::forward<PackedArguments>(packed_arguments));
    return Serialize(result);
  }
};

template <typename PackedArguments>
struct TypedMethod<void, PackedArguments> {
  template <typename M>
  static RPCBytes Invoke(M f, const RPCBytes& input) {
    auto packed_arguments = Deserialize<PackedArguments>(input);
    std::apply(std::move(f), std::forward<PackedArguments>(packed_arguments));
    return "";
  }
};

}  // namespace detail

//////////////////////////////////////////////////////////////////////

class TRPCServer {
 public:
  TRPCServer(){};

  TRPCServer(IRPCServerImplPtr server) : impl_(server) {
  }

  template <typename F>
  void RegisterMethod(const std::string& method, F f) {
    // Deduce method arguments and return type

    using ArgumentsTupleType = typename FunctionTraits<F>::ArgumentsTuple;
    using RetType = typename FunctionTraits<F>::ResultType;

    using TypedInvoker =
        typename detail::TypedMethod<RetType, ArgumentsTupleType>;

    auto invoker =
        [f = std::move(f)](const RPCBytes& input) mutable -> RPCBytes {
      return TypedInvoker::Invoke(std::move(f), input);
    };

    impl_->RegisterMethod(method, invoker);
  }

 private:
  IRPCServerImplPtr impl_;
};

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
  TRPCChannel(IRPCChannelImplPtr impl) : impl_(impl) {
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

  void Shutdown() {
    impl_->Shutdown();
  }

 private:
  IRPCChannelImplPtr impl_;
};

//////////////////////////////////////////////////////////////////////

class TRPCClient {
 public:
  TRPCClient() {
  }

  TRPCClient(IRPCClientImplPtr impl) : impl_(impl) {
  }

  TRPCChannel Dial(const std::string& peer) {
    return TRPCChannel(impl_->Dial(peer));
  }

 private:
  IRPCClientImplPtr impl_;
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl
