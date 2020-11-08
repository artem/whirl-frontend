#pragma once

#include <random>

#include <whirl/matrix/log/logger.hpp>

namespace whirl {

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
    // WHIRL_LLOG("Random number generated");
    return twister_();
  }

 private:
  Twister twister_;

  Logger logger_{"Random"};
};

}  // namespace whirl
