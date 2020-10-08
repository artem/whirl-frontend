#pragma once

#include <whirl/rpc/impl/id.hpp>

#include <string>
#include <optional>

namespace whirl::rpc {

using TraceId = std::string;

//////////////////////////////////////////////////////////////////////

// Fiber context

void SetThisFiberTraceId(TraceId id);

//////////////////////////////////////////////////////////////////////

// Thread context

struct TLTraceGuard {
  TLTraceGuard(TraceId id);
  ~TLTraceGuard();
};

//////////////////////////////////////////////////////////////////////

std::optional<TraceId> GetCurrentTraceId();
TraceId GetOrGenerateNewTraceId(RPCId id);

}  // namespace whirl::rpc
