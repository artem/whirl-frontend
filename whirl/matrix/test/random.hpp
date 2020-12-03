#pragma once

#include <random>

namespace whirl {

class Random {
 public:
  Random(size_t seed)
    : twister_(seed) {
  }

  // Random number in range [from, to]
  size_t Get(size_t from, size_t to) {
    return from + Generate() % (to - from + 1);
  }

 private:
  size_t Generate() {
    return twister_();
  }

 private:
  std::mt19937 twister_;
};

}  // namespace whirl
