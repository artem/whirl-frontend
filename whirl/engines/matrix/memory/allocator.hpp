#pragma once

#include <whirl/engines/matrix/memory/new.hpp>

#include <wheels/support/mmap_allocation.hpp>

#include <cstdlib>

namespace whirl::matrix {

struct BlockHeader {
  uint32_t size;  // Power of 2
  uint32_t canary;

  BlockHeader* next;
};

// Intrusive
class BlockList {
 public:
  void Push(BlockHeader* block) {
    block->next = head_;
    head_ = block;
  }

  BlockHeader* TryPop() {
    if (head_ == nullptr) {
      return nullptr;
    }
    BlockHeader* block = head_;
    head_ = head_->next;
    return block;
  }

  void HardReset() {
    head_ = nullptr;
  }

 private:
  BlockHeader* head_{nullptr};
};

class BlockCache {
  static const size_t kMaxClassIndex = 10;

 public:
  BlockHeader* TryAcquire(size_t class_index);
  void Release(BlockHeader* block);

  void HardReset();

 private:
  BlockList block_lists_[kMaxClassIndex + 1];
};

class MemoryAllocator : public IMemoryAllocator {
 public:
  MemoryAllocator();
  ~MemoryAllocator();

  // And initialize with zeroes
  void* Allocate(size_t bytes) override;
  void Free(void* addr) override;

  bool FromHere(void* addr) const;

  void Reset();

  size_t BytesAllocated() const;

 private:
  void* DoAllocate(size_t bytes);
  void ZeroFillTo(char* pos);
  char* AllocateNewBlock(size_t bytes_pow2);
  bool Overflow(size_t bytes) const;
  static char* WriteBlockHeader(char* addr, size_t size);
  wheels::MutableMemView GetSpan(BlockHeader* block);

 private:
  BlockCache cache_;

  wheels::MmapAllocation arena_;
  char* next_;
  char* zfilled_;
};

}  // namespace whirl::matrix
