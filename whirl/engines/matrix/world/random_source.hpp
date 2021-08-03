#pragma once

#include <random>

namespace whirl::matrix {

class RandomSource {
  // NB: Consistent across all platforms
  // https://eel.is/c++draft/rand.eng.mers
  using Impl = std::mt19937;

 public:
  using ResultType = Impl::result_type;

 public:
  RandomSource(ResultType seed) {
    Reset(seed);
  }

  void Reset(ResultType seed) {
    impl_.seed(seed);
  }

  ResultType Next() {
    return impl_();
  }

 private:
  Impl impl_;
};

}  // namespace whirl::matrix
