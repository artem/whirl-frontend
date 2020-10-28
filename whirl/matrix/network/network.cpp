#include <whirl/matrix/network/network.hpp>

#include <whirl/matrix/world/global.hpp>
#include <whirl/matrix/world/dice.hpp>

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

Link* Network::GetLinkTo(const std::string server) {
  return link_layer_.GetLink(CurrentActorName(), server);
}

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

  return NetSocket{this, GetLinkTo(address), client_id, server_id};
}

// Called from client socket dtor
void Network::DisconnectClient(NetEndpointId id) {
  WHIRL_FMT_LOG("Disconnect client endpoint: {}", id);
  GlobalHeapScope guard;
  RemoveEndpoint(id);
}

void Network::Step() {
  Link* link = link_layer_.FindLinkWithNextPacket();
  auto packet = link->ExtractNextPacket();

  auto dest_endpoint_it = endpoints_.find(packet.dest);

  if (dest_endpoint_it == endpoints_.end()) {
    // TODO
    WHIRL_LOG("Cannot deliver message <" << packet.message << ">: endpoint "
                                         << packet.dest << " disconnected");
    if (packet.IsData()) {
      SendResetPacket(link->GetOpposite(), packet.source);
    }
    return;
  }

  auto& endpoint = dest_endpoint_it->second;

  if (packet.IsData()) {
    WHIRL_FMT_LOG("Deliver message to {} (endpoint {}): <{}>", link->End(),
                  packet.dest, packet.message);
    endpoint.handler->HandleMessage(
        packet.message,
        LightNetSocket(link->GetOpposite(), packet.dest, packet.source));
  } else {
    WHIRL_FMT_LOG("Deliver reset message to {} (endpoint {})", link->End(),
                  packet.dest);
    endpoint.handler->HandleDisconnect();

    // endpoints_.erase(packet.to);
  }
}

// IFaultyNetwork

void Network::Split() {
  GlobalHeapScope g;

  // Generate partition
  std::vector<std::string> servers = link_layer_.Servers();

  Partition lhs;
  size_t lhs_size = GlobalRandomNumber(1, servers.size());

  for (size_t i = 0; i < lhs_size; ++i) {
    size_t k = GlobalRandomNumber(i, lhs_size);
    std::swap(servers[i], servers[k]);
    lhs.insert(servers[i]);
  }

  // Print
  WHIRL_FMT_LOG("Network partitioned: {} / {}", lhs.size(), servers.size() - lhs.size());

  // Split
  link_layer_.Split(lhs);
}

void Network::Heal() {
  GlobalHeapScope g;

  link_layer_.Heal();
  WHIRL_FMT_LOG("Network healed");
}

}  // namespace whirl
