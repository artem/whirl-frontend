#pragma once

#include <whirl/rpc/trace.hpp>

#include <any>
#include <memory>
#include <string>

namespace whirl::rpc {

// https://golang.org/pkg/context

//////////////////////////////////////////////////////////////////////

namespace detail {

struct ContextState {
  // TODO
};

}  // namespace detail

//////////////////////////////////////////////////////////////////////

struct Context {
  static Context Expired();
  static Context Background();
  static Context ThisFiber();
  static Context Default();

  bool IsExpired() {
    return static_cast<bool>(state.lock());
  }

  std::weak_ptr<detail::ContextState> state;
};

// Holder
class NewContext {
 public:
  NewContext()
    : state_(std::make_shared<detail::ContextState>()) {
  }

  Context Get() const {
    return {state_};
  }

  NewContext(NewContext&&) = default;
  NewContext& operator=(NewContext&&) = default;

  NewContext(const NewContext&) = delete;
  NewContext& operator=(const NewContext&) = delete;

 private:
  std::shared_ptr<detail::ContextState> state_;
};

}  // namespace whirl::rpc
