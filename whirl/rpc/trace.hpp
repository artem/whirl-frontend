#pragma once

#include <whirl/rpc/id.hpp>

#include <await/util/context.hpp>

#include <string>
#include <optional>

namespace whirl::rpc {

// Globally unique
using TraceId = std::string;

//////////////////////////////////////////////////////////////////////

// Fiber context

await::context::Context MakeTraceContext(TraceId trace_id);

std::optional<TraceId> TryGetTraceId(const await::context::Context& context);

//////////////////////////////////////////////////////////////////////

// Thread context

struct TLTraceContext {
  TLTraceContext(TraceId id);
  ~TLTraceContext();

  static std::optional<TraceId> TryGet();
};

//////////////////////////////////////////////////////////////////////

// Get current callback/fiber thread trace id
std::optional<TraceId> TryGetCurrentTraceId();

// Get current trace id or generate new based on request id
TraceId GetOrGenerateNewTraceId();

}  // namespace whirl::rpc
