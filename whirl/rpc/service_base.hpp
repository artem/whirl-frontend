#pragma once

#include <whirl/rpc/service.hpp>
#include <whirl/rpc/invoke_helper.hpp>

#include <wheels/support/preprocessor.hpp>
#include <wheels/support/assert.hpp>

#include <map>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

template <typename TService>
class ServiceBase : public IService {
 protected:
  using ThisService = TService;

 public:
  bool Has(const std::string& method_name) const override {
    return methods_.find(method_name) != methods_.end();
  }

  BytesValue Invoke(const std::string& method_name,
                    const BytesValue& input) override {
    auto method_it = methods_.find(method_name);
    WHEELS_VERIFY(method_it != methods_.end(),
                  "RPC method not found: " << method_name);

    auto& invoker = method_it->second;
    return invoker(input);
  }

  void Initialize() override {
    RegisterMethods();
  }

 protected:
  // Override this
  // Use WHIRL_RPC_REGISTER_METHOD to register methods
  virtual void RegisterMethods() = 0;

  template <typename R, typename... Args>
  void RegisterMethod(const std::string& method_name,
                      R (TService::*method)(Args...)) {
    TService* self = static_cast<TService*>(this);

    auto closure = [self, method](Args&&... args) -> R {
      return (self->*method)(std::forward<Args>(args)...);
    };

    auto invoker = [closure = std::move(closure)](
                       const BytesValue& input) mutable -> BytesValue {
      return detail::InvokeHelper<R, Args...>::Invoke(closure, input);
    };

    methods_.emplace(method_name, invoker);
  }

 private:
  using MethodInvoker = std::function<BytesValue(BytesValue)>;

  // Name -> Invoker
  std::map<std::string, MethodInvoker> methods_;
};

}  // namespace whirl::rpc

//////////////////////////////////////////////////////////////////////

#define WHIRL_RPC_REGISTER_METHOD(method) \
  RegisterMethod(TO_STRING(method), &ThisService::method)
