#include <whirl/engines/matrix/fault/net/star.hpp>

#include <whirl/engines/matrix/fault/accessors.hpp>

namespace whirl::matrix::fault {

void MakeStar(size_t center) {
  auto& net = Network();

  auto servers = net.ListServers();

  for (size_t i = 0; i < servers.size(); ++i) {
    for (size_t j = 0; j < servers.size(); ++j) {
      if (i != center && j != center) {
        net.PauseLink(servers[i], servers[j]);
      }
    }
  }
}

}  // namespace whirl::matrix::fault
