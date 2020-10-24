#pragma once

#include <whirl/rpc/impl/service.hpp>
#include <whirl/rpc/impl/invoke_helper.hpp>

#include <wheels/support/preprocessor.hpp>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

template <typename TService>
class RPCServiceBase : public IRPCService {
 protected:
  // See RPC_REGISTER_METHOD macro
  using ThisService = TService;

 public:
  bool Has(const std::string& method) const override {
    return methods_.find(method) != methods_.end();
  }

  BytesValue Invoke(const std::string& method, const BytesValue& input) override {
    auto method_it = methods_.find(method);
    WHEELS_VERIFY(method_it != methods_.end(), "RPC method not found: " << method);

    auto& invoker = method_it->second;
    return invoker(input);
  }

  void Initialize() override {
    RegisterRPCMethods();
  }

 protected:
  // Override this
  virtual void RegisterRPCMethods() {
    // Use RegisterRPCMethod
  }

  template <typename R, typename... Args>
  void RegisterRPCMethod(const std::string& name,
                         R (TService::*method)(Args...)) {
    TService* self = static_cast<TService*>(this);

    auto self_method = [self, method](Args&&... args) -> R {
      return (self->*method)(std::forward<Args>(args)...);
    };

    auto invoker = [method = std::move(self_method)](
                       const BytesValue& input) mutable -> BytesValue {
      return detail::InvokeHelper<R, Args...>::Invoke(method, input);
    };

    methods_.emplace(name, invoker);
  }

 private:
  using MethodInvoker = std::function<BytesValue(BytesValue)>;

  std::map<std::string, MethodInvoker> methods_;
};

}  // namespace whirl::rpc

//////////////////////////////////////////////////////////////////////

#define RPC_REGISTER_METHOD(method) \
  RegisterRPCMethod(TO_STRING(method), &ThisService::method)
