#pragma once

#include <wheels/support/hash_combine.hpp>

#include <cstdlib>
#include <functional>

namespace whirl {

class DigestCalculator {
  using Self = DigestCalculator;

 public:
  Self& Eat(size_t hash_value) {
    wheels::HashCombine(digest_, hash_value);
    return *this;
  }

  template <typename T>
  Self& EatT(const T& object) {
    return Eat(std::hash<T>()(object));
  }

  size_t Get() const {
    return digest_;
  }

 private:
  size_t digest_{0};
};

}  // namespace whirl
