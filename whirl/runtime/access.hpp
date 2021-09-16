#pragma once

#include <whirl/runtime/runtime.hpp>

namespace whirl::node {

// Bridge connecting engine-agnostic node and concrete engine
IRuntime& GetRuntime();

}  // namespace whirl::node
