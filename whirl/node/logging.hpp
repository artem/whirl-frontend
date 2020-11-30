#pragma once

#include <whirl/services/logger.hpp>

#include <fmt/core.h>

namespace whirl {

#ifndef NDEBUG

#define NODE_LOG_IMPL(level, ...) \
  NodeLogger()->Log(level, fmt::format(__VA_ARGS__))

#else

#define NODE_LOG_IMPL(level, ...)

#endif

#define NODE_LOG_DEBUG(...) NODE_LOG_IMPL(whirl::NodeLogLevel::Debug, __VA_ARGS__)
#define NODE_LOG_INFO(...)  NODE_LOG_IMPL(whirl::NodeLogLevel::Info, __VA_ARGS__)
#define NODE_LOG_ERROR(...) NODE_LOG_IMPL(whirl::NodeLogLevel::Error, __VA_ARGS__)

// Backward compatibility
#define NODE_LOG(...) NODE_LOG_INFO(__VA_ARGS__)

}  // namespace whirl
