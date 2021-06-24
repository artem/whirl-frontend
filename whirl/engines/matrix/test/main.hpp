#pragma once

#include <functional>

namespace whirl::matrix {

// Seed -> Digest
using Simulation = std::function<size_t(size_t)>;

int Main(int argc, const char** argv, Simulation sim);

}  // namespace whirl::matrix
