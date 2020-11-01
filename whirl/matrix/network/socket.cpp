#include <whirl/matrix/network/socket.hpp>

#include <whirl/matrix/network/link.hpp>
#include <whirl/matrix/network/transport.hpp>

#include <whirl/matrix/common/allocator.hpp>

namespace whirl::net {

//////////////////////////////////////////////////////////////////////

class ClientSocket::Impl {
 public:
  Impl(Transport* transport, Link* link, Port self, Port server,
               Timestamp ts)
               : transport_(transport), link_(link), self_port_(self), server_port_(server), ts_(ts) {
  }

  ~Impl() {
    transport_->RemoveEndpoint(self_port_);
  }

  const std::string& Peer() const {
    GlobalHeapScope g;
    return link_->EndHostName();
  }

  void Send(const Message& message) {
    GlobalHeapScope g;
    link_->Add(MakePacket(message));
  }

 private:
  Packet MakePacket(const Message& message) {
    return {EPacketType::Data, self_port_, message, server_port_, ts_};
  }

 private:
  Transport* transport_;
  Link* link_;
  Port self_port_;
  Port server_port_;
  Timestamp ts_;
};

ClientSocket::ClientSocket(Transport* transport, Link* link, Port self, Port server,
                           Timestamp ts)
    : impl_(std::make_unique<Impl>(transport, link, self, server, ts)) {
}

ClientSocket::~ClientSocket() {
}

const std::string& ClientSocket::Peer() const {
  return impl_->Peer();
}

void ClientSocket::Send(const Message& message) {
  impl_->Send(message);
}

void ClientSocket::Close() {
  impl_.reset();
}

bool ClientSocket::IsValid() const {
  return (bool)impl_;
}

//////////////////////////////////////////////////////////////////////

class ServerSocket::Impl {
 public:
  Impl(Transport* transport, Port port)
    : transport_(transport), port_(port) {
  }

  ~Impl() {
    transport_->RemoveEndpoint(port_);
  }

 private:
  Transport* transport_;
  Port port_;
};

ServerSocket::ServerSocket(Transport* transport, Port port)
  : impl_(std::make_unique<Impl>(transport, port)) {
}

ServerSocket::~ServerSocket() {
}

void ServerSocket::Close() {
  impl_.reset();
}

//////////////////////////////////////////////////////////////////////

ReplySocket::ReplySocket(Link* link, const Packet& packet)
    : link_(link),
      self_port_(packet.dest_port),
      peer_port_(packet.source_port),
      ts_(packet.ts) {
}

Packet ReplySocket::MakePacket(const Message& message) {
  return {EPacketType::Data, self_port_, message, peer_port_, ts_};
}

void ReplySocket::Send(const Message& message) {
  GlobalHeapScope g;
  link_->Add(MakePacket(message));
}

const std::string& ReplySocket::Peer() const {
  return link_->EndHostName();
}

}  // namespace whirl::net
