#include <whirl/rpc/trace.hpp>

#include <await/fibers/core/api.hpp>
#include <await/fibers/core/fls.hpp>

#include <wheels/support/assert.hpp>

// TODO: ???
#include <whirl/node/runtime.hpp>

#include <fmt/core.h>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

// Fiber context

void SetThisFiberTraceId(TraceId id) {
  await::fibers::self::SetLocal("rpc_trace_id", id);
}

std::optional<TraceId> TryGetThisFiberTraceId() {
  return await::fibers::self::GetLocal<TraceId>("rpc_trace_id");
}

//////////////////////////////////////////////////////////////////////

// Thread context

static thread_local std::optional<TraceId> rpc_trace_id;

TLTraceContext::TLTraceContext(TraceId id) {
  WHEELS_VERIFY(!await::fibers::AmIFiber(),
                "Thread-local trace guard used in fiber context");
  WHEELS_VERIFY(!rpc_trace_id.has_value(),
                "Cannot overwrite already set thread-local trace id");

  rpc_trace_id.emplace(id);
}

TLTraceContext::~TLTraceContext() {
  rpc_trace_id.reset();
}

std::optional<TraceId> TLTraceContext::TryGet() {
  return rpc_trace_id;
}

//////////////////////////////////////////////////////////////////////

using await::executors::IExecutor;
using await::executors::Task;

class TracingExecutor : public IExecutor {
 public:
  TracingExecutor(IExecutorPtr e, TraceId id)
      : e_(std::move(e)), id_(std::move(id)) {
  }

  void Execute(Task&& task) {
    auto wrapper = [id = id_, task = std::move(task)]() mutable {
      TLTraceContext context{id};
      task();
    };
    e_->Execute(std::move(wrapper));
  }

 private:
  IExecutorPtr e_;
  TraceId id_;
};

IExecutorPtr MakeTracingExecutor(IExecutorPtr e, TraceId id) {
  return std::make_shared<TracingExecutor>(std::move(e), std::move(id));
}

//////////////////////////////////////////////////////////////////////

std::optional<TraceId> TryGetCurrentTraceId() {
  if (await::fibers::AmIFiber()) {
    // Fiber local
    return TryGetThisFiberTraceId();
  } else {
    // Thread local
    return TLTraceContext::TryGet();
  }
}

// Globally unique!
static TraceId GenerateNewTraceId() {
  return GetRuntime().guids->Generate();
}

TraceId GetOrGenerateNewTraceId() {
  if (auto trace_id = TryGetCurrentTraceId()) {
    return *trace_id;
  }
  return GenerateNewTraceId();
}

}  // namespace whirl::rpc
