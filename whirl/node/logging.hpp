#pragma once

#include <whirl/services/logger.hpp>

#include <fmt/core.h>

namespace whirl {

#ifndef NDEBUG
#define NODE_LOG(...) NodeLogger()->Log(NodeLogLevel::Info, fmt::format(__VA_ARGS__))
#else
#define NODE_LOG(...)
#endif

}  // namespace whirl
