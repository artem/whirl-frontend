#include <whirl/matrix/network/socket.hpp>

#include <whirl/matrix/network/network.hpp>

#include <whirl/matrix/common/allocator.hpp>

namespace whirl {

NetSocket::NetSocket(Network* net, NetEndpointId self, NetEndpointId peer,
                     bool client)
    : self_(self), peer_(peer), net_(net), client_(client) {
}

NetSocket::~NetSocket() {
  Close();
}

void NetSocket::Close() {
  if (IsValid() && client_) {
    net_->DisconnectClient(self_);
    Invalidate();
  }
}

void NetSocket::Send(const Message& message) {
  GlobalHeapScope guard;
  Message copied(message);
  net_->SendMessage(self_, std::move(copied), peer_);
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
    net_->CloseServerSocket(self_);
  }
}

}  // namespace whirl
