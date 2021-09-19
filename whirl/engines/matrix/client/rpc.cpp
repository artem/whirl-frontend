#include <whirl/engines/matrix/client/main.hpp>

#include <whirl/engines/matrix/history/channel.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

#include <commute/rpc/client.hpp>

#include <commute/rpc/retries.hpp>

#include <whirl/node/rpc/random.hpp>

#include <fmt/core.h>

namespace whirl::matrix::client {

//////////////////////////////////////////////////////////////////////

static commute::rpc::BackoffParams RetriesBackoff() {
  return {50, 1000, 2};  // Magic
}

static ::commute::rpc::IClientPtr MakeRpcClient() {
  return ::commute::rpc::MakeClient(node::rt::NetTransport(),
                                    node::rt::Executor(),
                                    node::rt::LoggerBackend());
}

static std::string MakeAddress(const std::string& host, uint16_t port) {
  return fmt::format("{}:{}", host, port);
}

::commute::rpc::IChannelPtr MakeRpcChannel(const std::string& pool_name,
                                           uint16_t port) {
  auto pool = node::rt::Discovery()->ListPool(pool_name);

  auto client = MakeRpcClient();

  // Peer channels
  std::vector<::commute::rpc::IChannelPtr> transports;
  for (const auto& host : pool) {
    transports.push_back(client->Dial(MakeAddress(host, port)));
  }

  // Retries -> History -> Random -> Transport-s

  auto random =
      rpc::MakeRandomChannel(std::move(transports), node::rt::RandomService());
  auto history = MakeHistoryChannel(std::move(random));
  auto retries =
      commute::rpc::WithRetries(std::move(history), node::rt::TimeService(),
                                node::rt::LoggerBackend(), RetriesBackoff());

  return retries;
}

}  // namespace whirl::matrix::client
