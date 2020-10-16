#include <whirl/matrix/client/client.hpp>

#include <whirl/matrix/channels/random.hpp>
#include <whirl/matrix/channels/retries.hpp>
#include <whirl/matrix/channels/history.hpp>

namespace whirl {

rpc::IRPCChannelPtr ClientBase::MakeClientChannel() {
  // Peer channels
  std::vector<rpc::IRPCChannelPtr> channels;
  for (const auto& node : nodes_) {
    channels.push_back(services_.rpc_client.MakeChannel(node));
  }

  // Retries -> History -> Random -> Peers

  auto random = MakeRandomChannel(std::move(channels));
  auto history = MakeHistoryChannel(std::move(random));
  auto retries = WithRetries(std::move(history), TimeService());

  return retries;
}

}  // namespace whirl
