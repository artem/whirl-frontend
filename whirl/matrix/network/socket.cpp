#include <whirl/matrix/network/socket.hpp>

#include <whirl/matrix/network/network.hpp>

#include <whirl/matrix/common/allocator.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

LightNetSocket::LightNetSocket(Link* link, NetEndpointId self,
                               NetEndpointId peer)
    : self_(self), peer_(peer), link_(link) {
}

NetPacket LightNetSocket::MakePacket(const Message& message) {
  return {EPacketType::Data, self_, message, peer_};
}

void LightNetSocket::Send(const Message& message) {
  GlobalHeapScope g;
  link_->Add(MakePacket(message));
}

//////////////////////////////////////////////////////////////////////

NetSocket::NetSocket(Network* net, Link* link, NetEndpointId self,
                     NetEndpointId peer)
    : self_(self), peer_(peer), net_(net), link_(link) {
}

NetSocket::~NetSocket() {
  Close();
}

NetSocket NetSocket::Invalid() {
  return NetSocket{nullptr, nullptr, 0, 0};
}

void NetSocket::Close() {
  if (IsValid()) {
    net_->DisconnectClient(self_);
    Invalidate();
  }
}

NetPacket NetSocket::MakePacket(const Message& message) {
  return {EPacketType::Data, self_, message, peer_};
}

void NetSocket::Send(const Message& message) {
  GlobalHeapScope g;
  link_->Add(MakePacket(message));
}

bool NetSocket::IsValid() const {
  return !(self_ == 0 && peer_ == 0);
}

NetSocket::NetSocket(NetSocket&& that) {
  self_ = that.self_;
  peer_ = that.peer_;
  net_ = that.net_;
  link_ = that.link_;

  that.Invalidate();
}

void NetSocket::Invalidate() {
  net_ = nullptr;
  link_ = nullptr;
  self_ = peer_ = 0;
}

//////////////////////////////////////////////////////////////////////

NetServerSocket::NetServerSocket(Network* net, ServerAddress self)
    : self_(self), net_(net) {
}

NetServerSocket::NetServerSocket(NetServerSocket&& that) {
  self_ = that.self_;
  net_ = that.net_;
  that.net_ = nullptr;
}

NetServerSocket::~NetServerSocket() {
  if (net_) {
    net_->DisconnectServer(self_);
  }
}

}  // namespace whirl
