#include <whirl/node/cluster/peer.hpp>

#include <whirl/node/runtime/methods.hpp>

#include <commute/rpc/retries.hpp>

#include <fmt/core.h>

namespace whirl::node::cluster {

Peer::Peer(cfg::IConfig* config)
    : pool_name_(config->GetString("pool")),
      port_(config->GetInt<uint16_t>("rpc.port")) {
  ConnectToPeers(config);
}

size_t Peer::NodeCount() const {
  return pool_.size();
}

std::vector<std::string> Peer::ListPeers(bool with_me) const {
  if (with_me) {
    return pool_;
  } else {
    return others_;
  }
}

const ::commute::rpc::IChannelPtr& Peer::Channel(
    const std::string& peer) const {
  auto it = channels_.find(peer);
  return it->second;  // Or UB =(
}

const ::commute::rpc::IChannelPtr& Peer::LoopBack() const {
  return Channel(rt::HostName());
}

::commute::rpc::IClientPtr Peer::MakeRpcClient() {
  return ::commute::rpc::MakeClient(rt::NetTransport(), rt::Executor(),
                                    rt::LoggerBackend());
}

void Peer::ConnectToPeers(cfg::IConfig* cfg) {
  pool_ = rt::Discovery()->ListPool(pool_name_);

  auto client = MakeRpcClient();

  // others_ = pool_ \ {rt::HostName()}
  for (const auto& host : pool_) {
    if (host != rt::HostName()) {
      others_.push_back(host);
    }
  }

  for (const auto& host : pool_) {
    channels_.emplace(host, MakeRpcChannel(client, host, cfg));
  }
}

static commute::rpc::BackoffParams RetriesBackoff(cfg::IConfig* config) {
  return {config->GetInt<uint64_t>("rpc.backoff.init"),
          config->GetInt<uint64_t>("rpc.backoff.max"),
          config->GetInt<uint64_t>("rpc.backoff.factor")};
}

static std::string PeerAddress(const std::string& host, uint16_t port) {
  return fmt::format("{}:{}", host, port);
}

::commute::rpc::IChannelPtr Peer::MakeRpcChannel(
    ::commute::rpc::IClientPtr client, const std::string& host,
    cfg::IConfig* config) {
  auto port = config->GetInt<uint16_t>("rpc.port");
  auto transport = client->Dial(PeerAddress(host, port));
  auto retries =
      commute::rpc::WithRetries(std::move(transport), rt::TimeService(),
                                rt::LoggerBackend(), RetriesBackoff(config));
  return retries;
}

}  // namespace whirl::node::cluster
