#include <whirl/matrix/network/transport.hpp>

#include <whirl/matrix/network/network.hpp>

#include <whirl/matrix/common/allocator.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::net {

Transport::Transport(Network& net, const std::string& host, ProcessHeap& heap)
  : net_(net), host_(host), heap_(heap) {
}

ClientSocket Transport::ConnectTo(const Address& address, ISocketHandler* handler) {
  GlobalHeapScope g;

  Link* link = net_.GetLink(host_, address.host);

  Port port = FindFreePort();
  Timestamp ts = GetEndpointTimestamp();

  WHIRL_FMT_LOG("Connecting to {}: local port = {}, ts = {}", address, port, ts);

  endpoints_.emplace(port, Endpoint{handler, ts});

  /*
  // KeepAlive
  link->Add(
      {EPacketType::KeepAlive, port, "<keep-alive>", address.port, ts});
  */

  return ClientSocket{this, link, port, address.port, ts};
};

ServerSocket Transport::Serve(Port port, ISocketHandler* handler) {
  GlobalHeapScope g;

  WHEELS_VERIFY(endpoints_.count(port) == 0, "Port already in use");

  auto ts = GetEndpointTimestamp();
  endpoints_.emplace(port, Endpoint{handler, ts});

  WHIRL_FMT_LOG("Start serving at port {}, ts = {}", port, ts);

  return ServerSocket{this, port};
};

void Transport::RemoveEndpoint(Port port) {
  GlobalHeapScope g;

  WHIRL_FMT_LOG("Remove endpoint at port {}", port);
  endpoints_.erase(port);
}

void Transport::Reset() {
  GlobalHeapScope g;

  for (const auto& [port, _] : endpoints_) {
    WHIRL_FMT_LOG("Remove endpoint at port {}", port);
  }
  endpoints_.clear();
}


class Replier {
 public:
  Replier(const Packet& packet, Link* out)
    : packet_(packet), out_(out) {
  }

  void KeepAlive() {
    Send({EPacketType::KeepAlive, packet_.dest_port, "<keep-alive>", packet_.source_port, packet_.ts});
  }

  void Reset() {
    Send({EPacketType::Reset, packet_.dest_port, "<reset>", packet_.source_port, packet_.ts});
  }

  void Data(const Message& message) {
    Send({EPacketType::Data, packet_.dest_port, message, packet_.source_port, packet_.ts});
  }

 private:
  void Send(const Packet& packet) {
    out_->Add(packet);
  }

 private:
  Packet packet_;
  Link* out_;
};

void Transport::HandlePacket(const Packet& packet, Link* out) {
  GlobalHeapScope g;

  Address peer_address{out->EndHostName(), packet.source_port};

  WHIRL_FMT_LOG("Handle packet from {} with ts = {}", peer_address, packet.ts);

  Replier replier(packet, out);

  auto it = endpoints_.find(packet.dest_port);

  if (it == endpoints_.end()) {
    // Endpoint not found

    if (packet.type != EPacketType::Reset) {
      WHIRL_FMT_LOG("Endpoint at port {} not found, send reset packet to {}", packet.dest_port, peer_address);
      replier.Reset();
    }
    return;
  }

  const auto& endpoint = it->second;

  if (packet.ts < endpoint.ts) {
    WHIRL_FMT_LOG("Outdated packet, send <reset> packet to {}", peer_address);
    replier.Reset();

  } else if (packet.type == EPacketType::KeepAlive) {
    // KeepAlive
    WHIRL_FMT_LOG("Send keep-alive back to {}", peer_address);
    replier.KeepAlive();

  } else if (packet.type == EPacketType::Reset) {
    // Disconnect
    auto g = heap_.Use();
    endpoint.handler->HandleDisconnect(peer_address.host);

  } else if (packet.type == EPacketType::Data) {
    // Message
    auto g = heap_.Use();
    endpoint.handler->HandleMessage(packet.message,
        ReplySocket(out, packet));
  }
}

Port Transport::FindFreePort() {
  while (true) {
    if (endpoints_.count(next_port_) == 0) {
      return next_port_++;
    }
    next_port_++;
  }
}

}  // namespace whirl::net