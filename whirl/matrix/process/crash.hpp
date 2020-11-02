#pragma once

#include <whirl/matrix/process/heap.hpp>

namespace whirl {

// Dirty magic
void ReleaseFibersOnCrash(const ProcessHeap& heap);

}  // namespace whirl
