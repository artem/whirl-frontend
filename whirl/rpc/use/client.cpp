#include <whirl/rpc/use/client.hpp>

#include <whirl/rpc/impl/transport_channel.hpp>

namespace whirl::rpc {

TRPCChannel TRPCClient::Dial(const std::string& peer) {
  auto impl = std::make_shared<RPCTransportChannel>(transport_, executor_, peer);
  impl->Start();
  return TRPCChannel(impl);
}

}  // namespace whirl::rpc
