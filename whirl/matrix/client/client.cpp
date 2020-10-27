#include <whirl/matrix/client/client.hpp>

#include <whirl/matrix/channels/random.hpp>
#include <whirl/matrix/channels/retries.hpp>
#include <whirl/matrix/channels/history.hpp>

namespace whirl {

rpc::IChannelPtr ClientBase::MakeClientChannel() {
  // Peer channels
  std::vector<rpc::IChannelPtr> channels;
  for (const auto& addr : cluster_) {
    channels.push_back(services_.rpc_client.MakeChannel(addr));
  }

  // Retries -> History -> Random -> Peers

  auto random = MakeRandomChannel(std::move(channels));
  auto history = MakeHistoryChannel(std::move(random));
  auto retries = WithRetries(std::move(history), TimeService());

  return retries;
}

void ClientBase::Main() {
  await::fibers::self::SetName("main");

  RandomPause();
  DiscoverCluster();
  ConnectToClusterNodes();
  MainThread();
}

}  // namespace whirl
