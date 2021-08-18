#include <whirl/engines/matrix/client/client.hpp>

#include <whirl/rpc/random.hpp>
#include <whirl/rpc/retries.hpp>
#include <whirl/engines/matrix/history/channel.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

static rpc::BackoffParams RetriesBackoff() {
  return {50, 1000, 2};  // Magic
}

//////////////////////////////////////////////////////////////////////

rpc::IClientPtr ClientBase::MakeRpcClient() {
  return rpc::MakeClient(node::rt::NetTransport(), node::rt::Executor());
}

rpc::IChannelPtr ClientBase::MakeClientChannel() {
  // Peer channels
  std::vector<rpc::IChannelPtr> channels;
  for (const auto& addr : Cluster()) {
    channels.push_back(client_->Dial(addr));
  }

  // Retries -> History -> Random -> Peers

  auto random = rpc::MakeRandomChannel(std::move(channels), node::rt::RandomService());
  auto history = MakeHistoryChannel(std::move(random));
  auto retries =
      rpc::WithRetries(std::move(history), node::rt::TimeService(), RetriesBackoff());

  return retries;
}

void ClientBase::MainThread() {
  await::fibers::self::SetName("main");

  RandomPause();
  DiscoverCluster();
  ConnectToClusterNodes();
  MainRoutine();
}

}  // namespace whirl::matrix
