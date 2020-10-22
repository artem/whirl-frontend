#pragma once

#include <whirl/services/threads.hpp>

#include <functional>

namespace whirl::adversary {

using Strategy = std::function<void(ThreadsRuntime&)>;

}  // namespace whirl::adversary
