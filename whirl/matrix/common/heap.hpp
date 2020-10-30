#pragma once

#include <wheels/support/mmap_allocation.hpp>

#include <cstring>
#include <utility>
#include <optional>

namespace whirl {

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
 public:
  Heap();
  ~Heap();

  // And initialize with zeroes
  char* Allocate(size_t bytes);

  void Free(char* addr);

  bool FromHere(char* addr) const {
    return addr >= heap_.Start() && addr < heap_.End();
  }

  void Reset();

  size_t BytesAllocated() const {
    return next_ - heap_.Start();
  }

 private:
  void ZeroFillTo(char* pos);
  char* AllocateNewBlock(size_t bytes);
  static char* WriteBlockHeader(char* addr, size_t size);

 private:
  wheels::MmapAllocation heap_;
  char* next_;
  char* zfilled_;
};

}  // namespace whirl
