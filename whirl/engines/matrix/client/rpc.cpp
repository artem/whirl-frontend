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

rpc::IChannelPtr MakeRpcChannel(const std::string& pool_name) {
  auto pool = node::rt::Dns()->ListPool(pool_name);

  auto client = MakeRpcClient();

  // Peer channels
  std::vector<rpc::IChannelPtr> transports;
  for (const auto& host : pool) {
    transports.push_back(client->Dial(host));
  }

  // Retries -> History -> Random -> Transport-s

  auto random = rpc::MakeRandomChannel(std::move(transports), node::rt::RandomService());
  auto history = MakeHistoryChannel(std::move(random));
  auto retries =
      rpc::WithRetries(std::move(history), node::rt::TimeService(), RetriesBackoff());

  return retries;
}

}  // namespace whirl::matrix::client
