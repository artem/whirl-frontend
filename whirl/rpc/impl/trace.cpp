#include <whirl/rpc/impl/trace.hpp>

#include <await/fibers/core/fls.hpp>

#include <fmt/core.h>

namespace whirl::rpc {

void SetThisHandlerTraceId(TraceId id) {
  await::fibers::SetLocal("rpc_trace_id", id);
}

std::optional<TraceId> GetCurrentTraceId() {
  return await::fibers::GetLocal("rpc_trace_id");
}

TraceId GetOrGenerateNewTraceId(RPCId request_id) {
  auto trace_id = GetCurrentTraceId();
  if (trace_id) {
    return trace_id.value();
  }
  return fmt::format("R-{}", request_id);
}

}  // namespace whirl::rpc
