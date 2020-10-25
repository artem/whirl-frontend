#include <whirl/rpc/use/client.hpp>

#include <whirl/rpc/impl/transport_channel.hpp>

namespace whirl::rpc {

IChannelPtr TClient::MakeChannel(const std::string& peer) {
  auto impl =
      std::make_shared<TransportChannel>(transport_, executor_, peer);
  impl->Start();
  return impl;
}

}  // namespace whirl::rpc
