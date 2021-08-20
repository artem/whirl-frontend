#include <whirl/engines/matrix/client/main.hpp>

#include <whirl/engines/matrix/history/channel.hpp>

#include <whirl/runtime/methods.hpp>

#include <whirl/rpc/client.hpp>
#include <whirl/rpc/random.hpp>
#include <whirl/rpc/retries.hpp>

namespace whirl::matrix::client {

//////////////////////////////////////////////////////////////////////

static rpc::BackoffParams RetriesBackoff() {
  return {50, 1000, 2};  // Magic
}

static rpc::IClientPtr MakeRpcClient() {
  return rpc::MakeClient(node::rt::NetTransport(), node::rt::Executor());
}

rpc::IChannelPtr MakeRpcChannel() {
  auto cluster = node::rt::Dns()->ListPool("server");

  auto client = MakeRpcClient();

  // Peer channels
  std::vector<rpc::IChannelPtr> channels;
  for (const auto& addr : cluster) {
    channels.push_back(client->Dial(addr));
  }

  // Retries -> History -> Random -> Peers

  auto random = rpc::MakeRandomChannel(std::move(channels), node::rt::RandomService());
  auto history = MakeHistoryChannel(std::move(random));
  auto retries =
      rpc::WithRetries(std::move(history), node::rt::TimeService(), RetriesBackoff());

  return retries;
}

//////////////////////////////////////////////////////////////////////

static void RandomPause() {
  node::rt::SleepFor(node::rt::RandomNumber(50));
}

//////////////////////////////////////////////////////////////////////

void Prologue() {
  await::fibers::self::SetName("main");
  RandomPause();
}

}  // namespace whirl::matrix::client
