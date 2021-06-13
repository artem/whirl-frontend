#include <whirl/matrix/memory/allocator.hpp>

#include <whirl/matrix/memory/new.hpp>
#include <whirl/matrix/memory/heap.hpp>

#include <wheels/support/assert.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Fundamental alignment
// https://eel.is/c++draft/basic.align
static const size_t kRequiredAlignment = alignof(std::max_align_t);

static_assert(kRequiredAlignment == 16, "Unexpected!");

//////////////////////////////////////////////////////////////////////

struct BlockHeader {
  uint32_t size;
  uint32_t canary;

  uint64_t padding_;
};

static const size_t kBlockHeaderSize = sizeof(BlockHeader);

static_assert(kBlockHeaderSize % kRequiredAlignment == 0);

//////////////////////////////////////////////////////////////////////

// NB: No dynamic allocations here!

static size_t RoundUpTo16(size_t bytes) {
  return (bytes + 15) & ~15;
}

static const size_t kZFillBlockSize = 4096;

static const uint32_t kCanary = 23911147;

MemoryAllocator::MemoryAllocator() : heap_(AcquireHeap()) {
  WHEELS_VERIFY(heap_.Size() % kZFillBlockSize == 0,
                "Choose another kZFillBlockSize");
  Reset();
}

MemoryAllocator::~MemoryAllocator() {
  ReleaseHeap(std::move(heap_));
}

void* MemoryAllocator::Allocate(size_t bytes) {
  if (bytes > std::numeric_limits<uint32_t>::max()) {
    WHEELS_PANIC("Allocation is too large: " << bytes);
  }

  return AllocateNewBlock(bytes);
}

static BlockHeader* LocateBlockHeader(void* user_addr) {
  char* header_addr =(char*)user_addr - kBlockHeaderSize;
  return (BlockHeader*)header_addr;
}

void MemoryAllocator::Free(void* addr) {
  WHEELS_VERIFY(FromHere(addr), "Do not mess with heaps");
  BlockHeader* header = LocateBlockHeader(addr);
  WHEELS_VERIFY(header->canary == kCanary, "Memory allocator is broken");
}

void MemoryAllocator::Reset() {
  next_ = zfilled_ = heap_.Start();
}

void MemoryAllocator::ZeroFillTo(char* pos) {
  while (pos >= zfilled_) {
    std::memset(zfilled_, 0, kZFillBlockSize);
    zfilled_ += kZFillBlockSize;
  }
}

char* MemoryAllocator::AllocateNewBlock(size_t bytes) {
  bytes = RoundUpTo16(bytes);

  WHEELS_VERIFY(reinterpret_cast<uintptr_t>(next_) % kRequiredAlignment == 0,
                "Broken heap allocator");

  if (Overflow(bytes)) {
    GlobalAllocatorGuard g;
    WHEELS_PANIC("Cannot allocate " << bytes << " bytes: heap overflow");
  }

  // Incrementally fill heap with zeroes
  ZeroFillTo(next_ + kBlockHeaderSize + bytes);

  // printf("Allocate %zu bytes\n", bytes);
  char* user_addr = WriteBlockHeader(next_, bytes);
  next_ = user_addr + bytes;
  return user_addr;
}

bool MemoryAllocator::Overflow(size_t bytes) const {
  return next_ + kBlockHeaderSize + bytes >= heap_.End();
}

char* MemoryAllocator::WriteBlockHeader(char* addr, size_t size) {
  BlockHeader* header = (BlockHeader*)(addr);
  header->size = size;
  header->canary = kCanary;
  return addr + sizeof(BlockHeader);
};

bool MemoryAllocator::FromHere(void* addr) const {
  // Best effort
  return addr >= heap_.Start() && addr < heap_.End();
}

size_t MemoryAllocator::BytesAllocated() const {
  return next_ - heap_.Start();
}

}  // namespace whirl
