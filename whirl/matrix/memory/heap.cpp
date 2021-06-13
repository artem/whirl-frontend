#include <whirl/matrix/memory/heap.hpp>

#include <wheels/support/assert.hpp>

// TODO: remove
#include <whirl/matrix/memory/new.hpp>

#include <cstring>
#include <vector>

namespace whirl {

using wheels::MmapAllocation;

//////////////////////////////////////////////////////////////////////

static const size_t kPageSize = 4096;

//////////////////////////////////////////////////////////////////////

static const size_t kHeapSizeInPages = 16 * 1024;

//////////////////////////////////////////////////////////////////////

class HeapsAllocator {
 public:
  MmapAllocation Allocate() {
    if (!pool_.empty()) {
      auto heap = std::move(pool_.back());
      pool_.pop_back();
      return heap;
    }
    return AllocateNewHeap();
  }

  void Release(MmapAllocation heap) {
    pool_.push_back(std::move(heap));
  }

  void SetHeapSize(size_t bytes) {
    size_t pages = (bytes / kPageSize) + 1;
    heap_size_pages_ = pages;
  }

 private:
  MmapAllocation AllocateNewHeap() {
    return MmapAllocation::AllocatePages(heap_size_pages_);
  }

 private:
  size_t heap_size_pages_{kHeapSizeInPages};
  std::vector<MmapAllocation> pool_;
};

//////////////////////////////////////////////////////////////////////

static HeapsAllocator heaps;

MmapAllocation AcquireHeap() {
  return heaps.Allocate();
}

void ReleaseHeap(MmapAllocation heap) {
  heaps.Release(std::move(heap));
}

// Set before first simulation
void SetHeapSize(size_t bytes) {
  heaps.SetHeapSize(bytes);
}

}  // namespace whirl
