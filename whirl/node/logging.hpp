#pragma once

#include <fmt/core.h>

namespace whirl {

#define NODE_LOG(...) NodeLogger()->Log(fmt::format(__VA_ARGS__));

}  // namespace whirl
