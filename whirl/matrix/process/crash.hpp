#pragma once

#include <whirl/matrix/process/heap.hpp>

namespace whirl {

// Dirty magic
void ReleaseFiberResourcesOnCrash(const ProcessHeap& heap);

}  // namespace whirl
