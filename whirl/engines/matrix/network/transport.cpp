#include <whirl/engines/matrix/network/transport.hpp>

#include <whirl/engines/matrix/network/network.hpp>

#include <whirl/engines/matrix/memory/new.hpp>

// IsThereAdversary
#include <whirl/engines/matrix/world/global/global.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::net {

Transport::Transport(Network& net, const std::string& host, ProcessHeap& heap)
    : net_(net), host_(host), heap_(heap) {
}

ClientSocket Transport::ConnectTo(const Address& address,
                                  ISocketHandler* handler) {
  GlobalAllocatorGuard g;

  Link* link = net_.GetLink(host_, address.host);

  Port port = FindFreePort();
  Timestamp ts = GetNewEndpointTimestamp();

  WHIRL_SIM_LOG("Connecting to {}: local port = {}", address, port);

  endpoints_.emplace(port, Endpoint{handler, ts});

  if (IsThereAdversary()) {
    // Init ping-pong for detecting crashes / reboots
    link->Add({{EPacketType::Ping, port, address.port, ts}, "<ping>"});
  }

  {
    auto g = heap_.Use();
    return ClientSocket{this, link, port, address.port, ts};
  }
};

ServerSocket Transport::Serve(Port port, ISocketHandler* handler) {
  GlobalAllocatorGuard g;

  WHEELS_VERIFY(endpoints_.count(port) == 0, "Port already in use");

  auto ts = GetNewEndpointTimestamp();
  endpoints_.emplace(port, Endpoint{handler, ts});

  WHIRL_SIM_LOG("Start serving at port {}", port);

  {
    auto g = heap_.Use();
    return ServerSocket{this, port};
  }
};

void Transport::RemoveEndpoint(Port port) {
  GlobalAllocatorGuard g;

  WHIRL_SIM_LOG("Remove endpoint at port {}", port);
  endpoints_.erase(port);
}

void Transport::Reset() {
  GlobalAllocatorGuard g;

  for ([[maybe_unused]] const auto& [port, _] : endpoints_) {
    WHIRL_SIM_LOG("Remove endpoint at port {}", port);
  }
  endpoints_.clear();
}

class Replier {
 public:
  Replier(const Packet& packet, Link* out) : packet_(packet), out_(out) {
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
    Send({{type, packet_.header.dest_port, packet_.header.source_port,
          packet_.header.ts}, std::move(payload)});
  }

 private:
  const Packet packet_;
  Link* out_;
};

void Transport::HandlePacket(const Packet& packet, Link* out) {
  GlobalAllocatorGuard g;

  Address from{out->End()->HostName(), packet.header.source_port};
  Address to{host_, packet.header.dest_port};

  /*
  if (packet.type != EPacketType::Ping) {
    WHIRL_FMT_LOG("Handle packet from {} with ts = {}", from, packet.ts);
  }
  */

  Replier replier(packet, out);

  auto endpoint_it = endpoints_.find(packet.header.dest_port);

  if (endpoint_it == endpoints_.end()) {
    // Endpoint not found

    if (packet.header.type != EPacketType::Reset) {
      if (packet.header.type == EPacketType::Data) {
        WHIRL_SIM_LOG_WARN(
            "Endpoint {} not found, drop incoming packet from {}", to, from);
      }
      replier.Reset();
    }
    return;
  }

  const auto& endpoint = endpoint_it->second;

  if (packet.header.ts < endpoint.ts) {
    // WHIRL_FMT_LOG("Outdated packet, send <reset> packet to {}", from);
    replier.Reset();
    return;

  } else if (packet.header.type == EPacketType::Ping) {
    // Ping
    // WHIRL_FMT_LOG("Send ping back to {}", source);
    replier.Ping();
    return;

  } else if (packet.header.type == EPacketType::Reset) {
    // Disconnect
    auto g = heap_.Use();
    endpoint.handler->HandleDisconnect(from.host);
    return;

  } else if (packet.header.type == EPacketType::Data) {
    // Message

    WHIRL_SIM_LOG("Handle message at {} from {}: <{}>", host_, from,
                  packet.message);

    auto g = heap_.Use();
    endpoint.handler->HandleMessage(packet.message, ReplySocket(packet.header, out));
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
