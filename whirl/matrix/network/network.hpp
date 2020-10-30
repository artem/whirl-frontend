#pragma once

#include <whirl/matrix/network/address.hpp>
#include <whirl/matrix/network/endpoint_id.hpp>
#include <whirl/matrix/network/message.hpp>
#include <whirl/matrix/network/packet.hpp>
#include <whirl/matrix/network/socket.hpp>
#include <whirl/matrix/network/link_layer.hpp>
#include <whirl/matrix/network/connection.hpp>

#include <whirl/matrix/world/actor.hpp>
#include <whirl/matrix/world/faults.hpp>

#include <whirl/matrix/log/logger.hpp>

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

  using ClientConnections = std::map<NetEndpointId, Connection>;

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

  const std::string& Name() const override {
    static const std::string kName = "Network";
    return kName;
  }

  void Start() override;
  bool IsRunnable() const override;
  TimePoint NextStepTime() override;
  void Step() override;
  void Shutdown() override;

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

  void SendResetPacket(Link* link, NetEndpointId dest);

 private:
  Endpoints endpoints_;
  Servers servers_;
  LinkLayer link_layer_;
  ClientConnections conns_;

  wheels::support::IdGenerator endpoint_ids_;

  Logger logger_{"Network"};
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl
