#include <whirl/rpc/impl/trace.hpp>

#include <await/fibers/core/api.hpp>
#include <await/fibers/core/fls.hpp>

#include <wheels/support/assert.hpp>

#include <fmt/core.h>

namespace whirl::rpc {

using await::fibers::AmIFiber;

//////////////////////////////////////////////////////////////////////

// Fiber context

void SetThisFiberTraceId(TraceId id) {
  await::fibers::SetLocal("rpc_trace_id", id);
}

//////////////////////////////////////////////////////////////////////

// Thread context

static thread_local std::optional<TraceId> rpc_trace_id;

TLTraceGuard::TLTraceGuard(TraceId id) {
  WHEELS_VERIFY(!AmIFiber(), "Thread-local trace guard used in fiber context");
  WHEELS_VERIFY(!rpc_trace_id.has_value(), "Cannot overwrite already set thread-local trace id");

  rpc_trace_id.emplace(id);
}

TLTraceGuard::~TLTraceGuard() {
  rpc_trace_id.reset();
}

//////////////////////////////////////////////////////////////////////

std::optional<TraceId> GetCurrentTraceId() {
  if (AmIFiber()) {
    // Fiber local
    return await::fibers::GetLocal("rpc_trace_id");
  } else {
    // Thread local
    return rpc_trace_id;
  }
}

TraceId GetOrGenerateNewTraceId(RPCId request_id) {
  auto trace_id = GetCurrentTraceId();
  if (trace_id) {
    return trace_id.value();
  }
  return fmt::format("R-{}", request_id);
}

}  // namespace whirl::rpc
