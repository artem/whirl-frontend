#pragma once

#include <whirl/rpc/impl/id.hpp>

#include <await/executors/executor.hpp>

#include <string>
#include <optional>

namespace whirl::rpc {

using TraceId = std::string;

//////////////////////////////////////////////////////////////////////

// Fiber context

void SetThisFiberTraceId(TraceId id);

//////////////////////////////////////////////////////////////////////

// Thread context

struct TLTraceContext {
  TLTraceContext(TraceId id);
  ~TLTraceContext();

  static std::optional<TraceId> TryGet();
};

//////////////////////////////////////////////////////////////////////

using await::executors::IExecutorPtr;

IExecutorPtr MakeTracingExecutor(IExecutorPtr e, TraceId id);

//////////////////////////////////////////////////////////////////////

// Get current callback/fiber thread trace id
std::optional<TraceId> TryGetCurrentTraceId();

// Get current trace id or generate new based on request id
TraceId GetOrGenerateNewTraceId(RequestId id);

}  // namespace whirl::rpc
