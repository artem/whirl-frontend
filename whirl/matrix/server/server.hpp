#pragma once

#include <whirl/node/services.hpp>
#include <whirl/node/node.hpp>

#include <whirl/matrix/world/actor.hpp>
#include <whirl/matrix/world/faults.hpp>
#include <whirl/matrix/server/config.hpp>
#include <whirl/matrix/network/network.hpp>
#include <whirl/matrix/server/clocks.hpp>
#include <whirl/matrix/process/heap.hpp>
#include <whirl/matrix/process/network.hpp>
#include <whirl/matrix/common/event_queue.hpp>

#include <whirl/matrix/server/services/local_storage.hpp>

#include <whirl/matrix/log/logger.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class Server : public IActor, public IFaultyServer {
 public:
  Server(Network& network, ServerConfig config, INodeFactoryPtr factory)
      : config_(config),
        node_factory_(std::move(factory)),
        network_(network, Name()) {
  }

  // Non-copyable
  Server(const Server& that) = delete;
  Server& operator=(const Server& that) = delete;

  ~Server() {
    node_factory_.reset();
    Crash();
  }

  ServerAddress NetAddress() const {
    return Name();
  }

  // IFaultyServer

  void Crash() override;
  void Reboot() override;

  void Pause() override;
  void Resume() override;

  void AdjustWallClock() override;

  // IActor

  const std::string& Name() const override {
    return config_.name;
  }

  void Start() override;
  bool IsRunnable() const override;
  TimePoint NextStepTime() override;
  void Step() override;
  void Shutdown() override;

 private:
  NodeServices CreateNodeServices();

 private:
  ServerConfig config_;
  INodeFactoryPtr node_factory_;

  LocalWallClock wall_clock_;
  LocalMonotonicClock monotonic_clock_;
  LocalBytesStorage storage_;

  // Node process

  ProcessNetwork network_;
  mutable ProcessHeap heap_;

  EventQueue events_;
  bool paused_{false};  // TODO: better?

  Logger logger_{"Server"};
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl
