#include <whirl/matrix/memory/heap.hpp>

#include <wheels/support/assert.hpp>

#include <cstring>
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
    static const size_t kPages = 16 * 1024;
    return MmapAllocation::AllocatePages(kPages);
  }

 private:
  std::vector<MmapAllocation> pool_;
};

//////////////////////////////////////////////////////////////////////

static HeapsAllocator heaps;

static MmapAllocation AllocateHeapMemory() {
  return heaps.Allocate();
}

static void ReleaseHeapMemory(MmapAllocation heap) {
  heaps.Release(std::move(heap));
}

//////////////////////////////////////////////////////////////////////

static const size_t kZFillBlockSize = 4096;

Heap::Heap() : heap_(AllocateHeapMemory()) {
  WHEELS_VERIFY(heap_.Size() % kZFillBlockSize == 0,
                "Choose another kZFillBlockSize");
  Reset();
}

Heap::~Heap() {
  ReleaseHeapMemory(std::move(heap_));
}

char* Heap::Allocate(size_t bytes) {
  return AllocateNewBlock(bytes);
}

void Heap::Free(char* addr) {
  WHEELS_VERIFY(FromHere(addr), "Do not mess with heaps");
}

void Heap::Reset() {
  next_ = zfilled_ = heap_.Start();
}

void Heap::ZeroFillTo(char* pos) {
  while (pos >= zfilled_) {
    std::memset(zfilled_, 0, kZFillBlockSize);
    zfilled_ += kZFillBlockSize;
  }
}

char* Heap::AllocateNewBlock(size_t bytes) {
  WHEELS_VERIFY(next_ + 8 + bytes < heap_.End(), "Heap overflow");

  // Incrementally fill heap with zeroes
  ZeroFillTo(next_ + 8 + bytes);

  // printf("Allocate %zu bytes\n", bytes);
  char* user_addr = WriteBlockHeader(next_, bytes);
  next_ = user_addr + bytes;
  return user_addr;
}

char* Heap::WriteBlockHeader(char* addr, size_t size) {
  *reinterpret_cast<size_t*>(addr) = size;
  return addr + 8;
};

}  // namespace whirl
