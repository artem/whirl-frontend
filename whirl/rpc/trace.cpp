#include <whirl/rpc/trace.hpp>

#include <await/fibers/core/api.hpp>

#include <wheels/support/assert.hpp>

#include <await/executors/execute.hpp>

// TODO: ???
#include <whirl/runtime/runtime.hpp>

#include <fmt/core.h>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

// Fiber context

await::context::Context MakeTraceContext(TraceId trace_id) {
  return await::context::New().Set("trace_id", trace_id);
}

std::optional<TraceId> TryGetTraceId(const await::context::Context& context) {
  return context.TryGet<TraceId>("trace_id");
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
using await::executors::TaskBase;

class TracingExecutor : public IExecutor {
 public:
  TracingExecutor(IExecutorPtr e, TraceId id)
      : e_(std::move(e)), id_(std::move(id)) {
  }

  void Execute(TaskBase* task) {
    auto wrapper = [id = id_, task = std::move(task)]() mutable {
      TLTraceContext context{id};
      task->Run();
    };
    await::executors::Execute(e_, std::move(wrapper));
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
    return TryGetTraceId(await::context::ThisFiber());
  } else {
    // Thread local
    return TLTraceContext::TryGet();
  }
}

// Globally unique!
static TraceId GenerateNewTraceId() {
  return node::GetRuntime().GuidGenerator()->Generate();
}

TraceId GetOrGenerateNewTraceId() {
  if (auto trace_id = TryGetCurrentTraceId()) {
    return *trace_id;
  }
  return GenerateNewTraceId();
}

}  // namespace whirl::rpc
