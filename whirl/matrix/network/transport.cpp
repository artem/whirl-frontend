#include <whirl/matrix/network/transport.hpp>

#include <whirl/matrix/network/network.hpp>

#include <whirl/matrix/common/allocator.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::net {

Transport::Transport(Network& net, const std::string& host, ProcessHeap& heap)
  : net_(net), host_(host), heap_(heap), logger_{host} {
}

ClientSocket Transport::ConnectTo(const Address& address, ISocketHandler* handler) {
  GlobalHeapScope g;

  Link* link = net_.GetLink(host_, address.host);

  Port port = FindFreePort();
  Timestamp ts = GetEndpointTimestamp();

  WHIRL_FMT_LOG("Connecting to {}: local port = {}", address, port);

  endpoints_.emplace(port, Endpoint{handler, ts});

  // Init ping-pong
  link->Add(
      {EPacketType::Ping, port, "<ping>", address.port, ts});

  {
    auto g = heap_.Use();
    return ClientSocket{this, link, port, address.port, ts};
  }
};

ServerSocket Transport::Serve(Port port, ISocketHandler* handler) {
  GlobalHeapScope g;

  WHEELS_VERIFY(endpoints_.count(port) == 0, "Port already in use");

  auto ts = GetEndpointTimestamp();
  endpoints_.emplace(port, Endpoint{handler, ts});

  WHIRL_FMT_LOG("Start serving at port {}", port);

  {
    auto g = heap_.Use();
    return ServerSocket{this, port};
  }
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

  void Ping() {
    Reply(EPacketType::Ping, "<ping>");
  }

  void Reset() {
    Reply(EPacketType::Reset, "<reset>");
  }

  void Data(const Message& message) {
    Reply(EPacketType::Data, message);
  }

 private:
  void Send(const Packet& packet) {
    out_->Add(packet);
  }

  void Reply(EPacketType type, Message payload) {
    Send({type, packet_.dest_port, std::move(payload), packet_.source_port, packet_.ts});
  }

 private:
  const Packet packet_;
  Link* out_;
};

void Transport::HandlePacket(const Packet& packet, Link* out) {
  GlobalHeapScope g;

  Address from{out->EndHostName(), packet.source_port};
  Address to{host_, packet.dest_port};

  /*
  if (packet.type != EPacketType::Ping) {
    WHIRL_FMT_LOG("Handle packet from {} with ts = {}", from, packet.ts);
  }
  */

  Replier replier(packet, out);

  auto it = endpoints_.find(packet.dest_port);

  if (it == endpoints_.end()) {
    // Endpoint not found

    if (packet.type != EPacketType::Reset) {
      //WHIRL_FMT_LOG("Endpoint {} not found, send reset packet to {}", to, from);
      replier.Reset();
    }
    return;
  }

  const auto& endpoint = it->second;

  if (packet.ts < endpoint.ts) {
    //WHIRL_FMT_LOG("Outdated packet, send <reset> packet to {}", from);
    replier.Reset();
    return;

  } else if (packet.type == EPacketType::Ping) {
    // Ping
    // WHIRL_FMT_LOG("Send ping back to {}", source);
    replier.Ping();
    return;

  } else if (packet.type == EPacketType::Reset) {
    // Disconnect
    auto g = heap_.Use();
    endpoint.handler->HandleDisconnect(from.host);
    return;

  } else if (packet.type == EPacketType::Data) {
    // Message

    WHIRL_FMT_LOG("Handle message from {}: <{}>", from, packet.message);

    auto g = heap_.Use();
    endpoint.handler->HandleMessage(packet.message,
        ReplySocket(out, packet));
    return;
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