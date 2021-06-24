#pragma once

#include <whirl/engines/matrix/process/heap.hpp>

namespace whirl::matrix {

// Dirty magic
void ReleaseFiberResourcesOnCrash(const ProcessHeap& heap);

}  // namespace whirl::matrix
