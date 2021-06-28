#pragma once

#include <whirl/engines/matrix/process/threads.hpp>

#include <functional>

namespace whirl::matrix::adversary {

using Strategy = std::function<void(ThreadsRuntime&)>;

}  // namespace whirl::matrix::adversary
