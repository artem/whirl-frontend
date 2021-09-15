#include <whirl/engines/matrix/fault/net/star.hpp>

#include <whirl/node/runtime/methods.hpp>

#include <whirl/engines/matrix/fault/access.hpp>

namespace whirl::matrix::fault {

void MakeStar(std::vector<std::string> pool, size_t center) {
  auto& net = Network();

  for (size_t i = 0; i < pool.size(); ++i) {
    for (size_t j = 0; j < pool.size(); ++j) {
      if (i != center && j != center) {
        net.PauseLink(pool[i], pool[j]);
      }
    }
  }
}

void MakeRandomStar(std::vector<std::string> pool) {
  size_t center = node::rt::RandomNumber(pool.size());
  MakeStar(std::move(pool), center);
}

}  // namespace whirl::matrix::fault
