#pragma once

#include <functional>

namespace whirl {

// Seed -> Digest
using Simulation = std::function<size_t(size_t)>;

int MatrixMain(int argc, const char** argv, Simulation sim);

}  // namespace whirl
