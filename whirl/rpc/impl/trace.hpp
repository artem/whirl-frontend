#pragma once

#include <whirl/rpc/impl/id.hpp>

#include <string>
#include <optional>

namespace whirl::rpc {

using TraceId = std::string;

void SetThisHandlerTraceId(TraceId id);

std::optional<TraceId> GetCurrentTraceId();
TraceId GetOrGenerateNewTraceId(RPCId id);

}  // namespace whirl::rpc
