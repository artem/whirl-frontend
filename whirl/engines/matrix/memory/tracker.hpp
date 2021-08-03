#pragma once

#include <wheels/support/intrusive_list.hpp>

// No dynamic allocations allowed!

struct AllocRecord : public wheels::IntrusiveListNode<AllocRecord> {
  void* address;
  size_t size;
  size_t id;
};

class GlobalAllocTracker {
  static const size_t kAllocLimit = 100500;
 public:
  void Activate() {
    active_ = true;
  }

  ~GlobalAllocTracker() {
    free_list_.UnlinkAll();
    allocs_.UnlinkAll();
  }

  // Returns allocation id
  size_t Allocate(void* address, size_t size) {
    if (!active_) {
      return 0;
    }
    AllocRecord* alloc = GetRecord();
    alloc->address = address;
    alloc->size = size;
    alloc->id = ++next_id_;
    allocs_.PushBack(alloc);
    return alloc->id;
  }

  void Deallocate(void* address) {
    if (!active_) {
      return;
    }
    auto it = allocs_.begin();
    while (it != allocs_.end()) {
      AllocRecord* alloc = it->AsItem();
      if (alloc->address == address) {
        alloc->Unlink();
        free_list_.PushBack(alloc);
        break;
      }
      ++it;
    }
    // hmmmm...
  }

  void PrintReport() {
    printf("Active global allocations:\n");
    auto it = allocs_.begin();
    while (it != allocs_.end()) {
      printf("%p / %zu / id = %zu\n", it->address, it->size, it->id);
      ++it;
    }
  }

 private:
  AllocRecord* GetRecord() {
    if (!free_list_.IsEmpty()) {
      return free_list_.PopFront();
    }
    size_t i = next_index_++;
    return &pool_[i];
  }

 private:
  bool active_ = false;
  AllocRecord pool_[kAllocLimit];
  size_t next_index_ = 0;
  size_t next_id_ = 0;
  wheels::IntrusiveList<AllocRecord> free_list_;
  wheels::IntrusiveList<AllocRecord> allocs_;
};
