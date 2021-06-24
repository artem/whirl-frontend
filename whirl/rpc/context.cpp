#include <whirl/rpc/context.hpp>

#include <await/fibers/core/fls.hpp>

namespace whirl::rpc {

static NewContext bg;

Context Context::Expired() {
  return {};
}

Context Context::Background() {
  return bg.Get();
}

Context Context::ThisFiber() {
  auto ctx_opaque = await::fibers::self::GetLocalImpl("context");
  if (!ctx_opaque.has_value()) {
    auto new_ctx = std::make_shared<detail::ContextState>();
    await::fibers::self::SetLocal("context", new_ctx);
    return {new_ctx};
  }
  auto ctx = std::any_cast<std::shared_ptr<detail::ContextState>>(ctx_opaque);
  return {ctx};
}

Context Context::Default() {
  return Context::Background();
}

}  // namespace whirl::rpc