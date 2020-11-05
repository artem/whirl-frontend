#pragma once

#include <cstdlib>
#include <functional>

namespace whirl {

class DigestCalculator {
  using Self = DigestCalculator;
 public:
   Self& Eat(size_t hash_value) {
    HashCombine(digest_, hash_value);
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
  void HashCombine(size_t& digest, size_t hash_value) {
    // https://stackoverflow.com/questions/4948780/magic-number-in-boosthash-combine
    digest ^= (hash_value + 0x9e3779b9 + (digest << 6) + (digest >> 2));
  }

 private:
  size_t digest_{0};
};

}  // namespace whirl
