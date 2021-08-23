#include <whirl/engines/matrix/memory/allocator.hpp>

#include <whirl/engines/matrix/memory/new.hpp>
#include <whirl/engines/matrix/memory/heap.hpp>

#include <wheels/support/assert.hpp>

#include <wheels/support/bithacks.hpp>

#include <cstring>

#define WHIRL_ALLOC_PANIC(error) \
  do {                           \
    GlobalAllocatorGuard g;      \
    WHEELS_PANIC(error);         \
  } while (false)

#define WHIRL_ALLOC_VERIFY(cond, error) \
  do {                                  \
    if (!(cond)) {                      \
      WHIRL_ALLOC_PANIC(error);         \
    }                                   \
  } while (false)

namespace whirl::matrix {

static size_t AllocToPowerOf2(size_t bytes) {
  size_t pow2 = RoundUpToNextPowerOfTwo(bytes);
  if (pow2 >= 16) {
    return pow2;
  } else {
    return 16;
  }
}

static const size_t kLargeAllocClass = 0;

static size_t GetAllocClass(size_t bytes_pow2) {
  WHIRL_ALLOC_VERIFY(
      IsPowerOfTwo(bytes_pow2),
      "Memory allocator internal error: GetAllocClass expected ^2, value = "
          << bytes_pow2);

  switch (bytes_pow2) {
    case 16: return 1;
    case 32: return 2;
    case 64: return 3;
    case 128: return 4;
    case 256: return 5;
    case 512: return 6;
    case 1024: return 7;
    case 2048: return 8;
    case 4096: return 9;
    case 8192: return 10;
    default: return kLargeAllocClass;
  }
}

//////////////////////////////////////////////////////////////////////

// Fundamental alignment
// https://eel.is/c++draft/basic.align
static const size_t kRequiredAlignment = alignof(std::max_align_t);

static_assert(kRequiredAlignment == 16, "Unexpected!");

//////////////////////////////////////////////////////////////////////

static_assert(sizeof(BlockHeader) % kRequiredAlignment == 0);

//////////////////////////////////////////////////////////////////////

BlockHeader* BlockCache::TryAcquire(size_t class_index) {
  WHIRL_ALLOC_VERIFY(class_index <= kMaxClassIndex,
                     "Memory allocator internal error");
  return block_lists_[class_index].TryPop();
}

void BlockCache::Release(BlockHeader* block) {
  size_t class_index = GetAllocClass(block->size);
  WHIRL_ALLOC_VERIFY(class_index <= kMaxClassIndex,
                     "Memory allocator internal error");
  block_lists_[class_index].Push(block);
}

void BlockCache::HardReset() {
  for (size_t i = 0; i <= kMaxClassIndex; ++i) {
    block_lists_[i].HardReset();
  }
}

//////////////////////////////////////////////////////////////////////

// NB: No dynamic allocations here!

static const size_t kZFillBlockSize = 4096;

static const uint32_t kCanary = 23911147;

MemoryAllocator::MemoryAllocator() : arena_(AcquireHeap()) {
  WHEELS_VERIFY(arena_.Size() % kZFillBlockSize == 0,
                "Choose another kZFillBlockSize");
  Reset();
}

MemoryAllocator::~MemoryAllocator() {
  cache_.HardReset();
  ReleaseHeap(std::move(arena_));
}

static bool IsAllocationTooLarge(size_t bytes) {
  return bytes > std::numeric_limits<uint32_t>::max();
}

void* MemoryAllocator::Allocate(size_t bytes) {
  if (IsAllocationTooLarge(bytes)) {
    WHIRL_ALLOC_PANIC("Allocation is too large: " << bytes);
  }

  void* user_addr = DoAllocate(bytes);

  WHIRL_ALLOC_VERIFY(
      ((uintptr_t)user_addr) % kRequiredAlignment == 0,
      "Memory allocator internal error: allocated block is not aligned");

  return user_addr;
}

void* MemoryAllocator::DoAllocate(size_t bytes) {
  size_t bytes_pow2 = AllocToPowerOf2(bytes);

  size_t alloc_class = GetAllocClass(bytes_pow2);

  // Large allocation
  if (alloc_class == kLargeAllocClass) {
    return AllocateNewBlock(bytes_pow2);
  }

  // Try cache
  BlockHeader* block = cache_.TryAcquire(alloc_class);
  if (block != nullptr) {
    auto span = GetSpan(block);
    memset(span.Data(), 0, span.Size());
    return span.Data();
  }

  return AllocateNewBlock(bytes_pow2);
}

static BlockHeader* LocateBlockHeader(void* user_addr) {
  char* header_addr = (char*)user_addr - sizeof(BlockHeader);
  return (BlockHeader*)header_addr;
}

void MemoryAllocator::Free(void* addr) {
  WHIRL_ALLOC_VERIFY(FromHere(addr), "Do not mess with heaps");
  BlockHeader* header = LocateBlockHeader(addr);
  WHIRL_ALLOC_VERIFY(header->canary == kCanary, "Memory allocator is broken");

  size_t alloc_class = GetAllocClass(header->size);

  if (alloc_class != kLargeAllocClass) {
    cache_.Release(header);
  }
}

void MemoryAllocator::Reset() {
  next_ = zfilled_ = arena_.Start();
  cache_.HardReset();
}

void MemoryAllocator::ZeroFillTo(char* pos) {
  while (pos >= zfilled_) {
    std::memset(zfilled_, 0, kZFillBlockSize);
    zfilled_ += kZFillBlockSize;
  }
}

char* MemoryAllocator::AllocateNewBlock(size_t bytes) {
  WHIRL_ALLOC_VERIFY(
      reinterpret_cast<uintptr_t>(next_) % kRequiredAlignment == 0,
      "Memory allocator internal error: bump-pointer is not aligned");

  WHIRL_ALLOC_VERIFY(!Overflow(bytes),
                     "Cannot allocate " << bytes << " bytes: arena overflow");

  // Incrementally fill heap with zeroes
  ZeroFillTo(next_ + sizeof(BlockHeader) + bytes);

  // printf("Allocate %zu bytes\n", bytes);
  char* user_addr = WriteBlockHeader(next_, bytes);
  next_ = user_addr + bytes;
  return user_addr;
}

bool MemoryAllocator::Overflow(size_t bytes) const {
  return next_ + sizeof(BlockHeader) + bytes >= arena_.End();
}

char* MemoryAllocator::WriteBlockHeader(char* addr, size_t size) {
  BlockHeader* header = (BlockHeader*)(addr);
  header->size = size;
  header->canary = kCanary;
  return addr + sizeof(BlockHeader);
};

wheels::MutableMemView MemoryAllocator::GetSpan(BlockHeader* block) {
  char* start = (char*)block + sizeof(BlockHeader);
  return {start, block->size};
}

bool MemoryAllocator::FromHere(void* addr) const {
  return addr >= arena_.Start() && addr < arena_.End();
}

size_t MemoryAllocator::BytesAllocated() const {
  return next_ - arena_.Start();
}

}  // namespace whirl::matrix
