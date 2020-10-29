#include <whirl/rpc/impl/request_context.hpp>

#include <await/fibers/core/fls.hpp>

namespace whirl::rpc {

RequestContextPtr GetRequestContext() {
  auto context = await::fibers::self::GetLocalImpl("rpc_request_context");
  if (context.has_value()) {
    return std::any_cast<RequestContextPtr>(context);
  }
  return nullptr;
}

void SetRequestContext(const RequestMessage& request) {
  auto context = std::make_shared<RequestContext>(request);
  await::fibers::self::SetLocal("rpc_request_context", context);
}

}  // namespace whirl::rpc
