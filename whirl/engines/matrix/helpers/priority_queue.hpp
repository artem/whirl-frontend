#pragma once

#include <queue>

namespace whirl {

// Consistent extraction order across platforms / impls

template <typename T>
class PriorityQueue {
  static bool Equal(const T& lhs, const T& rhs) {
    return !(lhs < rhs) && !(rhs < lhs);
  }

  struct UniqueT {
    T item;
    size_t tie_braker;  // Unique

    bool operator<(const UniqueT& that) const {
      return item < that.item ||
             (Equal(item, that.item) && tie_braker < that.tie_braker);
    }
  };

  struct PriorityLess {
    // Largest priority on top
    bool operator()(const UniqueT& lhs, const UniqueT& rhs) const {
      return rhs < lhs;
    }
  };

 public:
  void Clear() {
    // No clear method in std::priority_queue
    decltype(items_) empty;
    std::swap(items_, empty);
  }

  bool IsEmpty() const {
    return items_.empty();
  }

  const T& Smallest() const {
    return items_.top().item;
  }

  T Extract() {
    // Safe: https://en.cppreference.com/w/cpp/container/priority_queue/pop
    auto top = std::move(const_cast<UniqueT&>(items_.top()));
    items_.pop();
    return std::move(top.item);
  }

  void Insert(T value) {
    items_.push(MakeUnique(std::move(value)));
  }

  size_t Size() const {
    return items_.size();
  }

 private:
  UniqueT MakeUnique(T&& value) {
    return {std::move(value), ++inserts_};
  }

 private:
  std::priority_queue<UniqueT, std::vector<UniqueT>, PriorityLess> items_;
  size_t inserts_{0};
};

}  // namespace whirl
