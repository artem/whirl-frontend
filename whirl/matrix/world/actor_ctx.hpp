#pragma once

#include <whirl/matrix/world/actor.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class ActorContext {
  using Ctx = ActorContext;

 public:
  class ScopeGuard {
   public:
    ScopeGuard(Ctx* ctx, IActor* actor) : ctx_(ctx), actor_(actor) {
      ctx_->current_ = actor;
    }

    IActor* operator->() {
      return actor_;
    }

    ~ScopeGuard() {
      ctx_->current_ = nullptr;
    }

   private:
    Ctx* ctx_;
    IActor* actor_;
  };

  ScopeGuard Scope(IActor* actor) {
    return ScopeGuard(this, actor);
  }

  IActor* Get() const {
    return current_;
  }

 private:
  IActor* current_;
};

}  // namespace whirl
