#pragma once

#include <random>

namespace whirl::matrix {

class RandomSource {
  // NB: Consistent across all platforms
  // https://eel.is/c++draft/rand.eng.mers
  using Twister = std::mt19937;

 public:
  using ResultType = Twister::result_type;

 public:
  RandomSource(ResultType seed) {
    Reset(seed);
  }

  void Reset(ResultType seed) {
    twister_.seed(seed);
  }

  ResultType Next() {
    return twister_();
  }

 private:
  Twister twister_;
};

}  // namespace whirl::matrix
