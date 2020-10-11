#pragma once

#include <whirl/matrix/network/address.hpp>
#include <whirl/matrix/network/endpoint_id.hpp>
#include <whirl/matrix/network/message.hpp>
#include <whirl/matrix/network/socket.hpp>
#include <whirl/matrix/network/packet.hpp>

#include <whirl/matrix/world/actor.hpp>
#include <whirl/matrix/world/clock.hpp>
#include <whirl/matrix/world/global.hpp>
#include <whirl/matrix/world/dice.hpp>

#include <whirl/matrix/common/event_queue.hpp>
#include <whirl/matrix/common/allocator.hpp>

#include <whirl/matrix/log/log.hpp>

#include <whirl/helpers/copy.hpp>

#include <whirl/helpers/id.hpp>

#include <map>
#include <vector>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class Network : public IActor {
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

      auto id = CreateNewEndpoint(handler);
      servers_.emplace(address, id);

      WHIRL_LOG("Serve address " << address << ", endpoint " << id);
    }
    return NetServerSocket(this, address);
  }

  // Called from server socket dtor
  void DisconnectServer(const ServerAddress& address) {
    GlobalHeapScope guard;

    auto id = servers_[address];
    WHIRL_FMT_LOG("Stop serve address {}, delete server endpoint {}", address, id);
    servers_.erase(address);
    RemoveEndpoint(id);
  }

  // Client

  // Called from actor fibers
  NetSocket ConnectTo(const ServerAddress& address,
                      INetSocketHandler* handler) {
    GlobalHeapScope guard;

    auto server_it = servers_.find(address);

    if (server_it == servers_.end()) {
      return NetSocket::Invalid();
    }

    NetEndpointId server_id = server_it->second;
    NetEndpointId client_id = CreateNewEndpoint(handler);

    return NetSocket{this, client_id, server_id};
  }

  // Called from client socket dtor
  void DisconnectClient(NetEndpointId id) {
    WHIRL_FMT_LOG("Disconnect client endpoint: {}", id);
    GlobalHeapScope guard;
    RemoveEndpoint(id);
  }

  // Send

  // Context: Server
  void SendMessage(NetEndpointId from, const Message& message,
                   NetEndpointId to) {
    GlobalHeapScope guard;

    WHIRL_FMT_LOG("Send {} -> {} message <{}>", from, to, message);
    Send({EPacketType::Data, from, MakeCopy(message), to});
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

  void Step() override {
    NetPacket packet = ExtractNextPacket();

    auto dest_endpoint_it = endpoints_.find(packet.dest);

    if (dest_endpoint_it == endpoints_.end()) {
      WHIRL_LOG("Cannot deliver message <" << packet.message << ">: endpoint "
                                           << packet.dest << " disconnected");
      if (packet.IsData()) {
        SendResetPacket(packet.source);
      }
      return;
    }

    auto& endpoint = dest_endpoint_it->second;

    if (packet.IsData()) {
      WHIRL_FMT_LOG("Deliver message to endpoint {}: <{}>", packet.dest,
                    packet.message);
      endpoint.handler->HandleMessage(
          packet.message, LightNetSocket(this, packet.dest, packet.source));
    } else {
      WHIRL_FMT_LOG("Deliver reset message to endpoint {}", packet.dest);
      endpoint.handler->HandleDisconnect();

      // endpoints_.erase(packet.to);
    }
  }

  void Shutdown() override {
    packets_.Clear();
    servers_.clear();
    endpoints_.clear();
  }

  // Statistics

  size_t PacketsSent() const {
    return packets_sent_;
  }

 private:
  NetEndpointId NewEndpointId() {
    return endpoint_ids_.NextId();
  }

  NetEndpointId CreateNewEndpoint(INetSocketHandler* handler) {
    auto id = NewEndpointId();
    endpoints_.emplace(id, Endpoint{handler});
    return id;
  }

  void RemoveEndpoint(NetEndpointId id) {
    endpoints_.erase(id);
  }

  NetPacket ExtractNextPacket() {
    return packets_.Extract().packet;
  }

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
    packets_.Insert(PacketEvent({packet, ChooseDeliveryTime()}));
    ++packets_sent_;
  }

 private:
  // State
  Endpoints endpoints_;
  Servers servers_;
  PacketQueue packets_;

  bool duplicates_{true};

  IdGenerator endpoint_ids_;

  // Statistics
  size_t packets_sent_{0};

  Logger logger_{"Network"};
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl
