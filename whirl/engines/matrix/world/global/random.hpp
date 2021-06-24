#pragma once

#include <cstdlib>
#include <vector>

#include <wheels/support/assert.hpp>

namespace whirl::matrix {

// Deterministic randomness

size_t StepRandomNumber();

size_t GlobalRandomNumber();

// [0, bound)
size_t GlobalRandomNumber(size_t bound);

// [lo, hi)
size_t GlobalRandomNumber(size_t lo, size_t hi);

// Ordered selection without repetitions
template <typename T>
std::vector<T> GlobalRandomSelect(std::vector<T> items, size_t k) {
  WHEELS_VERIFY(k <= items.size(), "K > items.size()");

  std::vector<T> selected;
  selected.reserve(k);

  for (size_t i = 0; i < k; ++i) {
    // j \in [i, items.size())
    int j = GlobalRandomNumber(i, items.size());
    std::swap(items[i], items[j]);
    selected.push_back(items[i]);
  }

  return selected;
}

}  // namespace whirl::matrix
