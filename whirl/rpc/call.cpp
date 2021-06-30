#include <whirl/rpc/call.hpp>

#include <await/fibers/core/api.hpp>

namespace whirl::rpc {

namespace detail {

await::StopToken Caller1::DefaultStopToken() {
  if (await::fibers::AmIFiber()) {
    return await::fibers::self::GetLifetimeToken();
  }
  return await::NeverStop();
}

TraceId Caller1::GetTraceId() {
  if (trace_id_.has_value()) {
    return *trace_id_;
  }
  return GetOrGenerateNewTraceId();
}

}  // namespace detail

}  // namespace whirl::rpc