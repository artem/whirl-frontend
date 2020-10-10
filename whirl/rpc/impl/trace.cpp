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

TLTraceContext::TLTraceContext(TraceId id) {
  WHEELS_VERIFY(!AmIFiber(), "Thread-local trace guard used in fiber context");
  WHEELS_VERIFY(!rpc_trace_id.has_value(), "Cannot overwrite already set thread-local trace id");

  rpc_trace_id.emplace(id);
}

TLTraceContext::~TLTraceContext() {
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


using await::executors::Task;
using await::executors::IExecutor;

class TracingExecutor : public IExecutor {
 public:
  TracingExecutor(IExecutorPtr e, TraceId id)
      : e_(e), id_(id) {
  }

  void Execute(Task&& task) {
    auto wrapper = [id = id_, task = std::move(task)]() mutable {
      TLTraceContext context(id);
      task();
    };
    e_->Execute(std::move(wrapper));
  }

 private:
  IExecutorPtr e_;
  TraceId id_;
};

IExecutorPtr MakeTracingExecutor(IExecutorPtr e, TraceId id) {
  return std::make_shared<TracingExecutor>(std::move(e), id);
}

}  // namespace whirl::rpc
