#include <whirl/rpc/impl/context.hpp>

#include <await/fibers/core/fls.hpp>

namespace whirl::rpc {

ContextPtr GetContext() {
  auto context = await::fibers::self::GetLocalImpl("rpc_context");
  if (context.has_value()) {
    return std::any_cast<ContextPtr>(context);
  }
  return nullptr;
}

void SetContext(RPCId id) {
  auto context = std::make_shared<Context>(id);
  await::fibers::self::SetLocal("rpc_context", context);
}

}  // namespace whirl::rpc
