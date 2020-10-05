#pragma once

#include <fmt/core.h>

namespace whirl {

#ifndef NDEBUG
#define NODE_LOG(...) NodeLogger()->Log(fmt::format(__VA_ARGS__))
#else
#define NODE_LOG(...)
#endif

}  // namespace whirl
