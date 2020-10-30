#pragma once

#include <wheels/support/mmap_allocation.hpp>
#include <wheels/support/assert.hpp>

#include <cstring>
#include <utility>
#include <optional>

namespace whirl {

//////////////////////////////////////////////////////////////////////

wheels::MmapAllocation AllocateHeapMemory();
void ReleaseHeapMemory(wheels::MmapAllocation heap);

//////////////////////////////////////////////////////////////////////

class Block {
 public:
  char* UserAddr() const {
    return addr_ + 8;
  }

  char* End() const {
    return UserAddr() + Size();
  }

  size_t Size() const {
    return *reinterpret_cast<size_t*>(addr_);
  }

  static Block FromUserAddr(char* user_addr) {
    return Block(user_addr - 8);
  }

 private:
  Block(char* real_addr) : addr_(real_addr) {
  }

 private:
  char* addr_;
};

//////////////////////////////////////////////////////////////////////

// Currently: naive bump-pointer allocator

class Heap {
  static const size_t kZFillBlockSize = 4096;

 public:
  Heap() : heap_(AllocateHeapMemory()) {
    WHEELS_VERIFY(heap_.Size() % kZFillBlockSize == 0, "Choose another zfill block size");
    Reset();
  }

  ~Heap() {
    ReleaseHeapMemory(std::move(heap_));
  }

  bool FromHere(char* addr) const {
    return addr >= heap_.Start() && addr < heap_.End();
  }

  // And initialize with zeroes
  char* Allocate(size_t bytes) {
    return AllocateNewBlock(bytes);
  }

  void Free(char* addr) {
    WHEELS_VERIFY(FromHere(addr), "Do not mess with heaps");
  }

  size_t BytesAllocated() const {
    return next_ - heap_.Start();
  }

  void Reset() {
    next_ = zfilled_ = heap_.Start();
  }

 private:
  void ZeroFillTo(char* pos) {
    while (pos >= zfilled_) {
      std::memset(zfilled_, 0, kZFillBlockSize);
      zfilled_ += kZFillBlockSize;
    }
  }

  char* AllocateNewBlock(size_t bytes) {
    WHEELS_VERIFY(next_ + 8 + bytes < heap_.End(), "Heap overflow");

    // Incrementally fill heap with zeroes
    ZeroFillTo(next_ + 8 + bytes);

    // printf("Allocate %zu bytes\n", bytes);
    char* user_addr = WriteBlockHeader(next_, bytes);
    next_ = user_addr + bytes;
    return user_addr;
  }

  static char* WriteBlockHeader(char* addr, size_t size) {
    *reinterpret_cast<size_t*>(addr) = size;
    return addr + 8;
  };

 private:
  wheels::MmapAllocation heap_;
  char* next_;
  char* zfilled_;
};

}  // namespace whirl
