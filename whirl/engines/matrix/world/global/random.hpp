#pragma once

#include <cstdlib>
#include <vector>

#include <wheels/support/assert.hpp>

namespace whirl::matrix {

// Deterministic randomness

uint64_t StepRandomNumber();

uint64_t GlobalRandomNumber();

// [0, bound)
uint64_t GlobalRandomNumber(uint64_t bound);

// [lo, hi)
uint64_t GlobalRandomNumber(uint64_t lo, uint64_t hi);

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
