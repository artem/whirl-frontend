#pragma once

#include <whirl/runtime/runtime.hpp>

#include <functional>

namespace whirl::node {

// Bridge connecting engine-agnostic node and concrete engine
IRuntime& GetRuntime();

//////////////////////////////////////////////////////////////////////

using EngineRuntime = std::function<IRuntime&()>;

void SetupRuntime(EngineRuntime getter);

}  // namespace whirl::node
