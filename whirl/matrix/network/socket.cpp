#include <whirl/matrix/network/socket.hpp>

#include <whirl/matrix/network/network.hpp>

#include <whirl/matrix/common/allocator.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

LightNetSocket::LightNetSocket(Network* net, NetEndpointId self,
                               NetEndpointId peer)
    : self_(self), peer_(peer), net_(net) {
}

void LightNetSocket::Send(const Message& message) {
  net_->SendMessage(self_, message, peer_);
}

//////////////////////////////////////////////////////////////////////

NetSocket::NetSocket(Network* net, NetEndpointId self, NetEndpointId peer)
    : self_(self), peer_(peer), net_(net) {
}

NetSocket::~NetSocket() {
  Close();
}

NetSocket NetSocket::Invalid() {
  return NetSocket{nullptr, 0, 0};
}

void NetSocket::Close() {
  if (IsValid()) {
    net_->DisconnectClient(self_);
    Invalidate();
  }
}

void NetSocket::Send(const Message& message) {
  net_->SendMessage(self_, message, peer_);
}

bool NetSocket::IsValid() const {
  return !(self_ == 0 && peer_ == 0);
}

NetSocket::NetSocket(NetSocket&& that) {
  self_ = that.self_;
  peer_ = that.peer_;
  net_ = that.net_;

  that.Invalidate();
}

void NetSocket::Invalidate() {
  net_ = nullptr;
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
