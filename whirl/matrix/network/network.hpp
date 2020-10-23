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
#include <whirl/matrix/world/faults.hpp>

#include <whirl/matrix/common/event_queue.hpp>
#include <whirl/matrix/common/allocator.hpp>

#include <whirl/matrix/log/logger.hpp>

#include <whirl/matrix/common/copy.hpp>

#include <wheels/support/id.hpp>

#include <map>
#include <vector>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class Network : public IActor, public IFaultyNetwork {
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
                        INetSocketHandler* handler);

  // Called from NetServerSocket dtor
  void DisconnectServer(const ServerAddress& address);

  // Client

  // Called from actor fibers
  NetSocket ConnectTo(const ServerAddress& address, INetSocketHandler* handler);

  // Called from NetSocket dtor
  void DisconnectClient(NetEndpointId id);

  // Send

  // Context: Server
  void SendMessage(NetEndpointId from, const Message& message,
                   NetEndpointId to);

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

  void Step() override;

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

  bool duplicates_{false};

  wheels::support::IdGenerator endpoint_ids_;

  // Statistics
  size_t packets_sent_{0};

  Logger logger_{"Network"};
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl
