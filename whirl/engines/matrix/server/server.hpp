#pragma once

#include <whirl/node/services.hpp>
#include <whirl/node/node.hpp>

#include <whirl/engines/matrix/world/actor.hpp>
#include <whirl/engines/matrix/fault/server.hpp>

#include <whirl/engines/matrix/server/config.hpp>
#include <whirl/engines/matrix/server/clocks.hpp>
#include <whirl/engines/matrix/server/storage.hpp>

#include <whirl/engines/matrix/network/server.hpp>
#include <whirl/engines/matrix/network/network.hpp>
#include <whirl/engines/matrix/network/transport.hpp>

#include <whirl/engines/matrix/process/heap.hpp>
#include <whirl/engines/matrix/process/step_queue.hpp>

#include <whirl/engines/matrix/log/logger.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class Server : public IActor, public IFaultyServer, public net::IServer {
 private:
  enum class State {
    Initial,
    Running,
    Paused,
    Crashed,
  };

 public:
  Server(net::Network& network, ServerConfig config, INodeFactoryPtr factory);

  // Non-copyable
  Server(const Server& that) = delete;
  Server& operator=(const Server& that) = delete;

  ~Server();

  // INetServer

  const std::string& HostName() const override {
    return config_.hostname;
  }

  void HandlePacket(const net::Packet& packet, net::Link* out) override;

  // IFaultyServer

  bool IsAlive() const override;

  void Crash() override;
  void FastReboot() override;

  void Pause() override;
  void Resume() override;

  void AdjustWallClock() override;

  // IActor

  const std::string& Name() const override;

  // Share with IFaultyServer
  void Start() override;

  bool IsRunnable() const override;
  TimePoint NextStepTime() const override;
  void Step() override;
  void Shutdown() override;

  // Digest

  size_t ComputeDigest() const;

 private:
  NodeServices CreateNodeServices();

 private:
  State state_{State::Initial};

  ServerConfig config_;
  INodeFactoryPtr node_factory_;

  WallClock wall_clock_;
  MonotonicClock monotonic_clock_;
  PersistentStorage persistent_storage_;

  // Node process

  mutable ProcessHeap heap_;
  net::Transport transport_;
  StepQueue* steps_{nullptr};

  Logger logger_{"Server"};
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl