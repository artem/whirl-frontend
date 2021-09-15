#include <whirl/engines/matrix/world/global/random.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::matrix {

// size_t GlobalRandomNumber() defined in global.cpp

uint64_t GlobalRandomNumber(uint64_t bound) {
  WHEELS_VERIFY(bound > 0, "bound = 0");
  return GlobalRandomNumber() % bound;
}

uint64_t GlobalRandomNumber(uint64_t lo, uint64_t hi) {
  WHEELS_VERIFY(lo <= hi, "Invalid range");
  return lo + GlobalRandomNumber(hi - lo);
}

}  // namespace whirl::matrix
