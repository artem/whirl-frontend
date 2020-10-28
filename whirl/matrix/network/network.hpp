#pragma once

#include <whirl/matrix/network/address.hpp>
#include <whirl/matrix/network/endpoint_id.hpp>
#include <whirl/matrix/network/message.hpp>
#include <whirl/matrix/network/packet.hpp>
#include <whirl/matrix/network/socket.hpp>
#include <whirl/matrix/network/link_network.hpp>

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
  struct Endpoint {
    INetSocketHandler* handler;
  };

  using Endpoints = std::map<NetEndpointId, Endpoint>;

  using Servers = std::map<ServerAddress, NetEndpointId>;

 public:
  Network() = default;

  Network(const Network& that) = delete;
  Network& operator=(const Network& that) = delete;

  // Build

  void AddServer(std::string name) {
    link_layer_.AddServer(name);
  }

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
    link_layer_.BuildLinks();
  }

  const std::string& Name() const override {
    static const std::string kName = "Network";
    return kName;
  }

  bool IsRunnable() const override {
    return link_layer_.IsRunnable();
  }

  TimePoint NextStepTime() override {
    Link* link = link_layer_.NextPacketLink();
    WHEELS_VERIFY(link, "No active links");
    return *(link->NextPacketTime());
  }

  void Step() override;

  void Shutdown() override {
    servers_.clear();
    endpoints_.clear();
    link_layer_.Shutdown();
  }

  // IFaultyNetwork

  void Split() override;
  void Heal() override;

 private:
  // Context: Server
  Link* GetLinkTo(const ServerAddress server);

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

  void SendResetPacket(Link* link, NetEndpointId to) {
    WHIRL_LOG("Send reset packet to endpoint " << to);
    link->Add({EPacketType::Reset, 0, "", to});
  }

 private:
  // State
  Endpoints endpoints_;
  Servers servers_;
  LinkNetwork link_layer_;

  wheels::support::IdGenerator endpoint_ids_;

  Logger logger_{"Network"};
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl
