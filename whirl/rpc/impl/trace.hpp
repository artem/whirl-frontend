#pragma once

#include <whirl/rpc/impl/id.hpp>

#include <await/executors/executor.hpp>

#include <string>
#include <optional>

namespace whirl::rpc {

using await::executors::IExecutorPtr;

using TraceId = std::string;

//////////////////////////////////////////////////////////////////////

IExecutorPtr MakeTracingExecutor(IExecutorPtr e, TraceId id);

//////////////////////////////////////////////////////////////////////

// Fiber context

void SetThisFiberTraceId(TraceId id);

//////////////////////////////////////////////////////////////////////

// Thread context

struct TLTraceContext {
  TLTraceContext(TraceId id);
  ~TLTraceContext();
};

//////////////////////////////////////////////////////////////////////

std::optional<TraceId> GetCurrentTraceId();
TraceId GetOrGenerateNewTraceId(RPCId id);

}  // namespace whirl::rpc
