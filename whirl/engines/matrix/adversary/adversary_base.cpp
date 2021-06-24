#include <whirl/engines/matrix/adversary/adversary_base.hpp>

#include <whirl/engines/matrix/fault/accessors.hpp>

#include <whirl/engines/matrix/world/global/global.hpp>

namespace whirl::matrix::adversary {

void AdversaryBase::Start() {
  Initialize();

  size_t cluster_size = GetPool("cluster").size();

  std::vector<IFaultyServer*> servers;
  for (size_t i = 0; i < cluster_size; ++i) {
    servers.push_back(&AccessFaultyServer(i));

    Threads().Spawn([this, i]() {
      AbuseServer(AccessFaultyServer(i));
    });
  }

  Threads().Spawn([this, servers]() {
    AbuseWholeCluster(servers);
  });

  Threads().Spawn([this]() {
    AbuseNetwork(AccessFaultyNetwork());
  });
}

void AdversaryBase::RandomPause(size_t lo, size_t hi) {
  Threads().SleepFor(GlobalRandomNumber(lo, hi));
}

}  // namespace whirl::matrix::adversary
