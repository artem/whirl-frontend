#include <whirl/rpc/call.hpp>

#include <await/fibers/core/api.hpp>

namespace whirl::rpc {

namespace detail {

Caller& Caller::Context(await::context::Context context) {
  auto trace_id = rpc::TryGetTraceId(context);
  if (trace_id.has_value()) {
    trace_id_.emplace(*trace_id);
  }
  stop_token_ = context.StopToken();
  return *this;
}

await::context::StopToken Caller::DefaultStopToken() {
  if (await::fibers::AmIFiber()) {
    return await::fibers::self::GetLifetimeToken();
  }
  return await::context::NeverStop();
}

TraceId Caller::GetTraceId() {
  if (trace_id_.has_value()) {
    return *trace_id_;
  }
  return GetOrGenerateNewTraceId();
}

}  // namespace detail

}  // namespace whirl::rpc