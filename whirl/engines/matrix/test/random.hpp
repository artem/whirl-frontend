#pragma once

#include <random>

namespace whirl::matrix {

class Random {
 public:
  Random(size_t seed) : twister_(seed) {
  }

  size_t Get(size_t min_value, size_t max_value) {
    return min_value + Generate() % (max_value - min_value + 1);
  }

 private:
  size_t Generate() {
    return twister_();
  }

 private:
  std::mt19937 twister_;
};

}  // namespace whirl::matrix
