#include <whirl/matrix/memory/heap.hpp>

#include <wheels/support/assert.hpp>

// TODO: remove
#include <whirl/matrix/memory/allocator.hpp>

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

static MmapAllocation AllocateHeapMemory() {
  return heaps.Allocate();
}

static void ReleaseHeapMemory(MmapAllocation heap) {
  heaps.Release(std::move(heap));
}

// Set before first simulation
void SetHeapSize(size_t bytes) {
  heaps.SetHeapSize(bytes);
}

//////////////////////////////////////////////////////////////////////

static const size_t kAlignment = 8;

static size_t RoundUpTo8(size_t bytes) {
  return (bytes + 7) & ~7;
}

static const size_t kBlockHeaderSize = 8;

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
  bytes = RoundUpTo8(bytes);

  WHEELS_VERIFY(reinterpret_cast<uintptr_t>(next_) % kAlignment == 0, "Broken heap allocator");

  if (Overflow(bytes)) {
    GlobalHeapScope g;
    WHEELS_PANIC("Cannot allocate " << bytes << " bytes: heap overflow");
  }

  // Incrementally fill heap with zeroes
  ZeroFillTo(next_ + kBlockHeaderSize + bytes);

  // printf("Allocate %zu bytes\n", bytes);
  char* user_addr = WriteBlockHeader(next_, bytes);
  next_ = user_addr + bytes;
  return user_addr;
}

bool Heap::Overflow(size_t bytes) const {
  return next_ + kBlockHeaderSize + bytes >= heap_.End();
}

char* Heap::WriteBlockHeader(char* addr, size_t size) {
  *reinterpret_cast<size_t*>(addr) = size;
  return addr + kBlockHeaderSize;
};

}  // namespace whirl
