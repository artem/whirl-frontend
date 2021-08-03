#pragma once

#include <cstdlib>

namespace whirl::matrix {

// Seed -> Digest
typedef size_t (*Simulation)(size_t);

int Main(int argc, const char** argv, Simulation sim);

}  // namespace whirl::matrix
