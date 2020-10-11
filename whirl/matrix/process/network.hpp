#pragma once

#include <whirl/matrix/network/network.hpp>
#include <whirl/matrix/common/allocator.hpp>

#include <whirl/matrix/log/log.hpp>
#include <whirl/matrix/common/copy.hpp>

#include <map>

namespace whirl {

// Этот слой существует лишь по той причине, что сокеты, созданные на куче
// процесса, не разрушаются при его крэше.

//////////////////////////////////////////////////////////////////////

using SocketFd = size_t;

static const size_t kInvalidSocketFd = 0;

//////////////////////////////////////////////////////////////////////

class ProcessNetwork;

// RAII objects

class ProcessSocket {
 public:
  ProcessSocket(ProcessNetwork* net, SocketFd fd, ServerAddress peer);
  ~ProcessSocket();

  static ProcessSocket Invalid() {
    return ProcessSocket{nullptr, kInvalidSocketFd, ""};
  }

  // Non-copyable
  ProcessSocket(const ProcessSocket& that) = delete;
  ProcessSocket& operator=(const ProcessSocket& that) = delete;

  // Movable
  ProcessSocket(ProcessSocket&& socket);
  ProcessSocket& operator=(ProcessSocket&& socket);

  bool IsValid() const {
    return fd_ != kInvalidSocketFd;
  }

  const ServerAddress& Peer() const {
    return peer_;
  }

  void Send(Message message);
  void Close();

 private:
  void Invalidate();

 private:
  ProcessNetwork* net_;
  SocketFd fd_;
  ServerAddress peer_;
};

class ProcessServerSocket {
 public:
  ProcessServerSocket(ProcessNetwork* net, SocketFd fd, ServerAddress self);
  ~ProcessServerSocket();

  // Non-copyable
  ProcessServerSocket(const ProcessServerSocket& that) = delete;
  ProcessServerSocket& operator=(const ProcessServerSocket& that) = delete;

  ProcessServerSocket(ProcessServerSocket&& that);

 private:
  ProcessNetwork* net_;
  SocketFd fd_;
  ServerAddress self_;
};

//////////////////////////////////////////////////////////////////////

class ProcessNetwork {
 public:
  ProcessNetwork(Network& net, ServerAddress address)
      : net_(net), address_(address) {
  }

  // On server reboot

  // Context: world
  void Reset() {
    clients_.clear();
    servers_.clear();
  }

  // Context: server
  ProcessSocket ConnectTo(ServerAddress address, INetSocketHandler* handler) {
    SocketFd fd = NextSocketFd();
    {
      GlobalHeapScope g;
      NetSocket socket = net_.ConnectTo(address, handler);
      if (!socket.IsValid()) {
        return ProcessSocket::Invalid();
      }
      clients_.emplace(fd, std::move(socket));
    }
    return ProcessSocket(this, fd, address);
  }

  // Context: Server
  void Send(SocketFd fd, const Message& message) {
    GlobalHeapScope g;
    GetClientSocket(fd).Send(message);
  }

  // Context: Server
  void CloseClient(SocketFd fd) {
    GlobalHeapScope g;
    clients_.erase(fd);
  }

  // Context: Server
  ProcessServerSocket Serve(INetSocketHandler* handler) {
    SocketFd fd = NextSocketFd();
    {
      GlobalHeapScope g;
      servers_.emplace(fd, net_.Serve(address_, handler));
    }
    return ProcessServerSocket(this, fd, MakeCopy(address_));
  }

  // Context: Server
  void CloseServer(SocketFd fd) {
    GlobalHeapScope g;
    servers_.erase(fd);
  }

 private:
  SocketFd NextSocketFd() {
    return ++next_fd_;
  }

 private:
  NetSocket& GetClientSocket(SocketFd fd) {
    auto socket_it = clients_.find(fd);
    WHEELS_VERIFY(socket_it != clients_.end(),
                  "Client socket with fd " << fd << " not found");
    return socket_it->second;
  }

 private:
  Network& net_;
  ServerAddress address_;

  SocketFd next_fd_{1};
  std::map<SocketFd, NetSocket> clients_;
  std::map<SocketFd, NetServerSocket> servers_;
};

}  // namespace whirl
