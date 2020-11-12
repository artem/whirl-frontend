#include <whirl/matrix/adversary/adversary_base.hpp>

#include <whirl/matrix/world/global/global.hpp>

namespace whirl::adversary {

void AdversaryBase::Start() {
  Initialize();

  std::vector<IFaultyServer*> servers;
  for (size_t i = 0; i < GetClusterSize(); ++i) {
    servers.push_back(&AccessFaultyServer(i));

    Threads().Spawn([this, i]() { AbuseServer(AccessFaultyServer(i)); });
  }

  Threads().Spawn([this, servers]() { AbuseWholeCluster(servers); });

  Threads().Spawn([this]() { AbuseNetwork(AccessFaultyNetwork()); });
}

void AdversaryBase::RandomPause(size_t lo, size_t hi) {
  Threads().SleepFor(GlobalRandomNumber(lo, hi));
}

}  // namespace whirl::adversary
