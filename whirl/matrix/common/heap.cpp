#include <whirl/matrix/common/heap.hpp>

#include <vector>

namespace whirl {

using wheels::MmapAllocation;

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

 private:
  MmapAllocation AllocateNewHeap() {
    static const size_t kPages = 1024 * 1024;
    return MmapAllocation::AllocatePages(kPages);
  }

 private:
  std::vector<MmapAllocation> pool_;
};

//////////////////////////////////////////////////////////////////////

static HeapsAllocator heaps;

MmapAllocation AllocateHeapMemory() {
  return heaps.Allocate();
}

void ReleaseHeapMemory(MmapAllocation heap) {
  heaps.Release(std::move(heap));
}

}  // namespace whirl
