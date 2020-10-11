#include <whirl/matrix/network/network.hpp>

namespace whirl {

NetServerSocket Network::Serve(const ServerAddress& address,
                               INetSocketHandler* handler) {
  {
    GlobalHeapScope guard;
    WHEELS_VERIFY(servers_.count(address) == 0, "Address already in use");

    auto id = CreateNewEndpoint(handler);
    servers_.emplace(address, id);

    WHIRL_LOG("Serve address " << address << ", endpoint " << id);
  }
  return NetServerSocket(this, address);
}

// Called from server socket dtor
void Network::DisconnectServer(const ServerAddress& address) {
  GlobalHeapScope guard;

  auto id = servers_[address];
  WHIRL_FMT_LOG("Stop serve address {}, delete server endpoint {}", address,
                id);
  servers_.erase(address);
  RemoveEndpoint(id);
}

// Client

// Called from actor fibers
NetSocket Network::ConnectTo(const ServerAddress& address,
                             INetSocketHandler* handler) {
  GlobalHeapScope guard;

  auto server_it = servers_.find(address);

  if (server_it == servers_.end()) {
    return NetSocket::Invalid();
  }

  NetEndpointId server_id = server_it->second;
  NetEndpointId client_id = CreateNewEndpoint(handler);

  return NetSocket{this, client_id, server_id};
}

// Called from client socket dtor
void Network::DisconnectClient(NetEndpointId id) {
  WHIRL_FMT_LOG("Disconnect client endpoint: {}", id);
  GlobalHeapScope guard;
  RemoveEndpoint(id);
}

// Context: Server
void Network::SendMessage(NetEndpointId from, const Message& message,
                          NetEndpointId to) {
  GlobalHeapScope guard;

  WHIRL_FMT_LOG("Send {} -> {} message <{}>", from, to, message);
  Send({EPacketType::Data, from, MakeCopy(message), to});
}

void Network::Step() {
  NetPacket packet = ExtractNextPacket();

  auto dest_endpoint_it = endpoints_.find(packet.dest);

  if (dest_endpoint_it == endpoints_.end()) {
    WHIRL_LOG("Cannot deliver message <" << packet.message << ">: endpoint "
                                         << packet.dest << " disconnected");
    if (packet.IsData()) {
      SendResetPacket(packet.source);
    }
    return;
  }

  auto& endpoint = dest_endpoint_it->second;

  if (packet.IsData()) {
    WHIRL_FMT_LOG("Deliver message to endpoint {}: <{}>", packet.dest,
                  packet.message);
    endpoint.handler->HandleMessage(
        packet.message, LightNetSocket(this, packet.dest, packet.source));
  } else {
    WHIRL_FMT_LOG("Deliver reset message to endpoint {}", packet.dest);
    endpoint.handler->HandleDisconnect();

    // endpoints_.erase(packet.to);
  }
}

}  // namespace whirl
