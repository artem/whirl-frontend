#pragma once

#include <wheels/support/mmap_allocation.hpp>

#include <utility>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Set before first simulation
void SetHeapSize(size_t bytes);

wheels::MmapAllocation AcquireHeap();
void ReleaseHeap(wheels::MmapAllocation heap);

}  // namespace whirl
