#pragma once

#include <whirl/logger/log.hpp>

// Backward compatibility

#define WHIRL_SIM_LOG_DEBUG(...) \
  WHIRL_LOG_DEBUG(__VA_ARGS__)

#define WHIRL_SIM_LOG_INFO(...) \
  WHIRL_LOG_INFO(__VA_ARGS__)

#define WHIRL_SIM_LOG_WARN(...) \
  WHIRL_LOG_WARN(__VA_ARGS__)

#define WHIRL_SIM_LOG_ERROR(...) \
  WHIRL_LOG_ERROR(__VA_ARGS__)

#define WHIRL_SIM_LOG(...) WHIRL_LOG_INFO(__VA_ARGS__)