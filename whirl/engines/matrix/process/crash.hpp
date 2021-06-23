#pragma once

#include <whirl/engines/matrix/process/heap.hpp>

namespace whirl {

// Dirty magic
void ReleaseFiberResourcesOnCrash(const ProcessHeap& heap);

}  // namespace whirl
