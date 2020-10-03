#pragma once

#include <random>

#include <whirl/matrix/log/log.hpp>

namespace whirl {

class RandomSource {
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
  std::mt19937 twister_;

  Logger logger_{"Random"};
};

}  // namespace whirl
