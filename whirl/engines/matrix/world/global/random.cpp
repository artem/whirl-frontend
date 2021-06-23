#include <whirl/engines/matrix/world/global/random.hpp>

#include <wheels/support/assert.hpp>

namespace whirl {

// size_t GlobalRandomNumber() defined in global.cpp

size_t GlobalRandomNumber(size_t bound) {
  WHEELS_VERIFY(bound > 0, "bound = 0");
  return GlobalRandomNumber() % bound;
}

size_t GlobalRandomNumber(size_t lo, size_t hi) {
  WHEELS_VERIFY(lo <= hi, "Invalid range");
  return lo + GlobalRandomNumber(hi - lo);
}

}  // namespace whirl
