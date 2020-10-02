#include <whirl/matrix/process_network.hpp>

namespace whirl {

ProcessSocket::ProcessSocket(ProcessNetwork* net, SocketFd fd,
                             ServerAddress peer)
    : net_(net), fd_(fd), peer_(peer) {
}

ProcessSocket::~ProcessSocket() {
  if (IsValid()) {
    Close();
  }
}

ProcessSocket::ProcessSocket(ProcessSocket&& that) {
  net_ = that.net_;
  fd_ = that.fd_;
  peer_ = that.peer_;

  that.fd_ = kInvalidSocketFd;
}

ProcessSocket& ProcessSocket::operator=(ProcessSocket&& that) {
  if (IsValid()) {
    net_->CloseClient(fd_);
  }

  net_ = that.net_;
  fd_ = that.fd_;
  peer_ = that.peer_;

  that.fd_ = kInvalidSocketFd;

  return *this;
}

void ProcessSocket::Send(Message message) {
  WHEELS_VERIFY(IsValid(), "Invalid socket")
  net_->Send(fd_, message);
}

void ProcessSocket::Close() {
  WHEELS_VERIFY(IsValid(), "Invalid socket")
  net_->CloseClient(fd_);
  Invalidate();
}

void ProcessSocket::Invalidate() {
  fd_ = kInvalidSocketFd;
}

ProcessServerSocket::ProcessServerSocket(ProcessNetwork* net, SocketFd fd,
                                         ServerAddress self)
    : net_(net), fd_(fd), self_(self) {
}

ProcessServerSocket::~ProcessServerSocket() {
  if (fd_ != kInvalidSocketFd) {
    net_->CloseServer(fd_);
  }
}

ProcessServerSocket::ProcessServerSocket(ProcessServerSocket&& that) {
  net_ = that.net_;
  fd_ = that.fd_;
  self_ = that.self_;
  that.fd_ = kInvalidSocketFd;
}

}  // namespace whirl
