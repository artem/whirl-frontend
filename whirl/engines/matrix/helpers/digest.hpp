#pragma once

#include <wheels/support/hash.hpp>

#include <cstdlib>
#include <functional>

namespace whirl {

class DigestCalculator {
  using Self = DigestCalculator;

 public:
  Self& Combine(size_t hash_value) {
    wheels::HashCombine(digest_, hash_value);
    return *this;
  }

  template <typename T>
  Self& Eat(const T& object) {
    return Combine(std::hash<T>()(object));
  }

  size_t GetValue() const {
    return digest_;
  }

 private:
  size_t digest_{0};
};

}  // namespace whirl
