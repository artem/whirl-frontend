#pragma once

#include <set>

namespace whirl {

template <typename T>
class PriorityQueue {
 public:
  void Clear() {
    items_.clear();
  }

  bool IsEmpty() const {
    return items_.empty();
  }

  const T& Smallest() const {
    return *items_.begin();
  }

  T Extract() {
    return std::move(items_.extract(items_.begin()).value());
  }

  void Insert(T value) {
    items_.insert(std::move(value));
  }

 private:
  std::multiset<T> items_;
};

}  // namespace whirl
