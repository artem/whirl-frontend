#pragma once

#include <queue>

namespace whirl {

template <typename T>
class PriorityQueue {
  struct TPriorityLess {
    // Largest priority on top
    bool operator()(const T& lhs, const T& rhs) const {
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
    return items_.top();
  }

  T Extract() {
    // Safe: https://en.cppreference.com/w/cpp/container/priority_queue/pop
    auto item = std::move(const_cast<T&>(items_.top()));
    items_.pop();
    return std::move(item);
  }

  void Insert(T value) {
    items_.push(std::move(value));
  }

  size_t Size() const {
    return items_.size();
  }

 private:
  std::priority_queue<T, std::vector<T>, TPriorityLess> items_;
};

}  // namespace whirl
