#pragma once

#include <whirl/rpc/impl/transport_server.hpp>

#include <wheels/support/function_meta.hpp>
#include <wheels/support/preprocessor.hpp>

#include <cereal/types/tuple.hpp>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

// Typed method helpers

namespace detail {

template <typename Result, typename PackedArguments>
struct TypedMethod {
  template <typename M>
  static BytesValue Invoke(M f, const BytesValue& input) {
    auto packed_arguments = Deserialize<PackedArguments>(input);
    Result result = std::apply(std::move(f),
                               std::forward<PackedArguments>(packed_arguments));
    return Serialize(result);
  }
};

template <typename PackedArguments>
struct TypedMethod<void, PackedArguments> {
  template <typename M>
  static BytesValue Invoke(M f, const BytesValue& input) {
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

  TRPCServer(ITransportPtr t, IExecutorPtr e)
      : impl_(std::make_shared<RPCTransportServer>(t, e)) {
  }

  void Start() {
    impl_->Start();
  }

  // Non-copyable
  TRPCServer(const TRPCServer& that) = delete;
  TRPCServer& operator=(const TRPCServer& that) = delete;

  TRPCServer(TRPCServer&& that) = default;
  TRPCServer& operator=(TRPCServer&& that) = default;


  template <typename F>
  void RegisterHandler(const std::string& name, F&& f) {
    // Deduce method arguments and return type

    using ArgumentsTupleType = typename FunctionTraits<F>::ArgumentsTuple;
    using RetType = typename FunctionTraits<F>::ResultType;

    using TypedMethod =
        typename detail::TypedMethod<RetType, ArgumentsTupleType>;

    auto invoker =
        [f = std::move(f)](const BytesValue& input) mutable -> BytesValue {
      return TypedMethod::Invoke(std::move(f), input);
    };

    impl_->RegisterMethod(name, std::move(invoker));
  }

  template <typename T, typename R, typename ... Args>
  void RegisterMethod(const std::string& name, R(T::*method)(Args...), T* object) {

    auto binded_method = [object, method](Args&& ... args) -> R {
      return (object->*method)(std::forward<Args>(args)...);
    };

    RegisterHandler(name, std::move(binded_method));
  }

 private:
  std::shared_ptr<RPCTransportServer> impl_;
};

}  // namespace whirl::rpc

#define RPC_REGISTER_METHOD(cls, method) \
  rpc_server.RegisterMethod(TO_STRING(method), &cls::method, this)
