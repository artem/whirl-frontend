#pragma once

#include <whirl/matrix/clock.hpp>
#include <whirl/matrix/message.hpp>
#include <whirl/matrix/event_queue.hpp>
#include <whirl/matrix/actor.hpp>
#include <whirl/matrix/allocator.hpp>

#include <whirl/matrix/log.hpp>

#include <whirl/helpers/id.hpp>

#include <map>
#include <vector>

namespace whirl {

//////////////////////////////////////////////////////////////////////

using NetEndpointId = Id;

//////////////////////////////////////////////////////////////////////

using ServerAddress = std::string;

//////////////////////////////////////////////////////////////////////

class Network;

//////////////////////////////////////////////////////////////////////

// Sockets

struct NetSocket {
 public:
  NetSocket(Network* net, NetEndpointId self, NetEndpointId peer,
            bool client = false);
  ~NetSocket();

  static NetSocket Invalid() {
    return NetSocket{nullptr, 0, 0};
  }

  // Non-copyable
  NetSocket(const NetSocket& that) = delete;
  NetSocket& operator=(const NetSocket& that) = delete;

  // Movable
  NetSocket(NetSocket&& that);

  bool IsValid() const;
  void Send(const Message& message);
  void Close();

 private:
  void Invalidate();

 private:
  NetEndpointId self_;
  NetEndpointId peer_;
  Network* net_;
  bool client_;
};

class NetServerSocket {
 public:
  NetServerSocket(Network* net, ServerAddress self);
  ~NetServerSocket();

  // Non-copyable
  NetServerSocket(const NetServerSocket& that) = delete;
  NetServerSocket operator=(const NetServerSocket& that) = delete;

  NetServerSocket(NetServerSocket&& that);

 private:
  ServerAddress self_;
  Network* net_;
};

//////////////////////////////////////////////////////////////////////

struct INetSocketHandler {
  virtual ~INetSocketHandler() = default;

  virtual void HandleMessage(const Message& message, NetSocket back) = 0;

  // Peer disconnected
  virtual void HandleLost() = 0;
};

//////////////////////////////////////////////////////////////////////

class Network : public IActor {
  enum EPacketType { Data, Reset };

  struct NetPacket {
    EPacketType type;
    NetEndpointId from;
    Message message;
    NetEndpointId to;

    bool IsDataMessage() const {
      return type == EPacketType::Data;
    }
  };

  struct PacketEvent {
    NetPacket packet;
    TimePoint time;

    bool operator<(const PacketEvent& that) const {
      return time < that.time;
    }
  };

  using PacketQueue = PriorityQueue<PacketEvent>;

  struct Endpoint {
    INetSocketHandler* handler;
  };

  using Endpoints = std::map<NetEndpointId, Endpoint>;

  using Servers = std::map<ServerAddress, NetEndpointId>;

 public:
  Network(WorldClock& world_clock) : world_clock_(world_clock) {
  }

  Network(const Network& that) = delete;
  Network& operator=(const Network& that) = delete;

  // Server

  NetServerSocket Serve(const ServerAddress& address,
                        INetSocketHandler* handler) {
    {
      GlobalHeapScope guard;
      WHEELS_VERIFY(servers_.count(address) == 0, "Address already in use");
      auto id = NewEndpointId();
      endpoints_[id] = {handler};
      servers_[address] = id;

      WHIRL_LLOG("Serve address " << address << ", endpoint " << id);
    }
    return NetServerSocket(this, address);
  }

  // Called from server socket dtor
  void CloseServerSocket(const ServerAddress& address) {
    GlobalHeapScope guard;
    WHIRL_LLOG("Stop serve address " << address);
    auto server_id = servers_[address];
    servers_.erase(address);
    endpoints_.erase(server_id);
  }

  // Client

  // Called from actor fibers
  NetSocket ConnectTo(const ServerAddress& address,
                      INetSocketHandler* handler) {
    GlobalHeapScope guard;

    if (servers_.count(address) == 0) {
      return NetSocket::Invalid();
    }

    WHEELS_VERIFY(servers_.count(address) == 1, "Cannot connect");
    NetEndpointId server_id = servers_[address];

    auto client_id = NewEndpointId();
    WHIRL_LLOG("Create client endpoint: " << client_id);
    endpoints_[client_id] = {handler};

    return NetSocket(this, client_id, server_id, /*client=*/true);
  }

  // Called from client socket dtor
  void DisconnectClient(NetEndpointId client_id) {
    WHIRL_LLOG("Disconnect client endpoint: " << client_id);
    GlobalHeapScope guard;
    endpoints_.erase(client_id);
  }

  // Send

  void SendMessage(NetEndpointId from, Message message, NetEndpointId to) {
    WHIRL_LLOG("Send message <" << message << "> from " << from << " to "
                                << to);
    Send({EPacketType::Data, from, message, to});
  }

  // IActor

  const std::string& Name() const override {
    static const std::string kName = "Network";
    return kName;
  }

  bool IsRunnable() const override {
    return !packets_.IsEmpty();
  }

  TimePoint NextStepTime() override {
    return packets_.Smallest().time;
  }

  void MakeStep() override {
    NetPacket packet = packets_.Extract().packet;

    if (endpoints_.count(packet.to) == 0) {
      WHIRL_LLOG("Cannot deliver message <" << packet.message << ">: endpoint "
                                            << packet.to << " disconnected");
      if (packet.IsDataMessage()) {
        SendReset(packet.from);
      }
      return;
    }

    auto& endpoint = endpoints_[packet.to];

    if (packet.type == EPacketType::Data) {
      WHIRL_LLOG("Deliver message <" << packet.message << "> to endpoint "
                                     << packet.to);
      endpoint.handler->HandleMessage(packet.message,
                                      NetSocket(this, packet.to, packet.from));
    } else {
      WHIRL_LLOG("Deliver reset message to endpoint " << packet.to);
      endpoint.handler->HandleLost();
      endpoints_.erase(packet.to);
    }
  }

  void Shutdown() override {
    packets_.Clear();
    servers_.clear();
    endpoints_.clear();
  }

 private:
  void SendReset(NetEndpointId to) {
    WHIRL_LLOG("Send reset packet to endpoint " << to);
    Send({EPacketType::Reset, 0, "", to});
  }

  void Send(NetPacket packet) {
    TimePoint delivery_time = world_clock_.Now() + 1;
    packets_.Insert(PacketEvent({packet, delivery_time}));
  }

  NetEndpointId NewEndpointId() {
    return next_endpoint_id_.NextId();
  }

 private:
  WorldClock& world_clock_;

  IdGenerator next_endpoint_id_;
  PacketQueue packets_;
  Endpoints endpoints_;
  Servers servers_;

  Logger logger_{"Network"};
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl
