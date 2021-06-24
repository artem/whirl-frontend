#pragma once

#include <whirl/node/threads.hpp>

#include <functional>

namespace whirl::matrix::adversary {

using Strategy = std::function<void(ThreadsRuntime&)>;

}  // namespace whirl::matrix::adversary
