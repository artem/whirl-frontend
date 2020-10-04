#pragma once

#include <whirl/matrix/network/address.hpp>
#include <whirl/matrix/network/endpoint_id.hpp>
#include <whirl/matrix/network/message.hpp>
#include <whirl/matrix/network/socket.hpp>

#include <whirl/matrix/world/clock.hpp>
#include <whirl/matrix/world/global.hpp>
#include <whirl/matrix/world/dice.hpp>

#include <whirl/matrix/network/message.hpp>
#include <whirl/matrix/common/event_queue.hpp>
#include <whirl/matrix/world/actor.hpp>
#include <whirl/matrix/common/allocator.hpp>

#include <whirl/matrix/log/log.hpp>

#include <whirl/helpers/copy.hpp>

#include <whirl/helpers/id.hpp>

#include <map>
#include <vector>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class Network : public IActor {
  enum EPacketType { Data, Reset };

  struct NetPacket {
    EPacketType type;
    NetEndpointId from;
    Message message;
    NetEndpointId to;

    bool IsData() const {
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
  Network() {
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

      WHIRL_LOG("Serve address " << address << ", endpoint " << id);
    }
    return NetServerSocket(this, address);
  }

  // Called from server socket dtor
  void CloseServerSocket(const ServerAddress& address) {
    GlobalHeapScope guard;
    WHIRL_LOG("Stop serve address " << address);
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
    WHIRL_LOG("Create client endpoint: " << client_id);
    endpoints_[client_id] = {handler};

    return NetSocket(this, client_id, server_id);
  }

  // Called from client socket dtor
  void DisconnectClient(NetEndpointId client_id) {
    WHIRL_LOG("Disconnect client endpoint: " << client_id);
    GlobalHeapScope guard;
    endpoints_.erase(client_id);
  }

  // Send

  // Context: Server
  void SendMessage(NetEndpointId from, const Message& message, NetEndpointId to) {
    GlobalHeapScope guard;

    auto message_copy = MakeCopy(message);
    WHIRL_LOG("Send message <" << message_copy << "> from " << from << " to " << to);
    Send({EPacketType::Data, from, message_copy, to});
  }

  // IActor

  void Start() override {
    // Nop
  }

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
      WHIRL_LOG("Cannot deliver message <" << packet.message << ">: endpoint "
                                           << packet.to << " disconnected");
      if (packet.IsData()) {
        SendResetPacket(packet.from);
      }
      return;
    }

    auto& endpoint = endpoints_[packet.to];

    if (packet.type == EPacketType::Data) {
      WHIRL_LOG("Deliver message <" << packet.message << "> to endpoint "
                                    << packet.to);
      endpoint.handler->HandleMessage(packet.message,
                                      LightNetSocket(this, packet.to, packet.from));
    } else {
      WHIRL_LOG("Deliver reset message to endpoint " << packet.to);
      endpoint.handler->HandlePeerLost();
      endpoints_.erase(packet.to);
    }
  }

  void Shutdown() override {
    packets_.Clear();
    servers_.clear();
    endpoints_.clear();
  }

 private:
  void SendResetPacket(NetEndpointId to) {
    WHIRL_LOG("Send reset packet to endpoint " << to);
    Send({EPacketType::Reset, 0, "", to});
  }

  TimePoint ChooseDeliveryTime() {
    return GlobalNow() + NetPacketDeliveryTime();
  }

  void Send(NetPacket packet) {
    DoSend(packet);

    if (duplicates_ && DuplicateNetPacket()) {
      DoSend(packet);
    }
  }

  void DoSend(NetPacket packet) {
    packets_.Insert(
        PacketEvent({packet, ChooseDeliveryTime()}));
  }

  NetEndpointId NewEndpointId() {
    return next_endpoint_id_.NextId();
  }

 private:
  IdGenerator next_endpoint_id_;
  PacketQueue packets_;
  Endpoints endpoints_;
  Servers servers_;

  bool duplicates_{true};

  Logger logger_{"Network"};
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl
