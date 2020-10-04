#pragma once

#include <whirl/node/services.hpp>
#include <whirl/node/node.hpp>

#include <whirl/matrix/world/actor.hpp>
#include <whirl/matrix/network/network.hpp>
#include <whirl/matrix/server/clocks.hpp>
#include <whirl/matrix/process/heap.hpp>
#include <whirl/matrix/process/network.hpp>
#include <whirl/matrix/common/event_queue.hpp>

// Services

#include <whirl/matrix/server/services/executor.hpp>
#include <whirl/matrix/server/services/time.hpp>
#include <whirl/matrix/server/services/local_storage.hpp>
#include <whirl/matrix/server/services/true_time.hpp>
#include <whirl/matrix/server/services/rpc.hpp>
#include <whirl/matrix/server/services/uid.hpp>
#include <whirl/matrix/server/services/random.hpp>
#include <whirl/services/rpc.hpp>
#include <whirl/matrix/server/services/logger.hpp>

#include <whirl/matrix/log/log.hpp>

#include <await/fibers/core/api.hpp>
#include <await/futures/future.hpp>
#include <await/futures/promise.hpp>
#include <await/executors/executor.hpp>

#include <memory>

namespace whirl {

using namespace await::fibers;

//////////////////////////////////////////////////////////////////////

class Server : public IActor {
 public:
  Server(Network& network, NodeConfig config, INodeFactoryPtr factory)
      : config_(config),
        node_factory_(std::move(factory)),
        name_(MakeName(config)),
        network_(network, name_) {
    config_.name = Name();
    Create();
  }

  // Non-copyable
  Server(const Server& that) = delete;
  Server& operator=(const Server& that) = delete;

  ~Server() {
    node_factory_.reset();
    Crash();
  }

  ServerAddress NetAddress() {
    return Name();
  }

  void Reboot() {
    Crash();
    Create();
    Start();
  }

  void Pause() {
    WHEELS_VERIFY(!paused_, "Server already paused");
    paused_ = true;
  }

  void Resume() {
    WHEELS_VERIFY(paused_, "Server is not paused");

    auto now = GlobalNow();
    auto g = heap_.Use();

    while (!events_.IsEmpty() && events_.NextEventTime() < now) {
      auto pending_event = events_.TakeNext();
      events_.Add(now, std::move(pending_event.action));
    }

    paused_ = false;
  }

  void AdjustWallTime() {
    local_wall_time_clock_.AdjustOffset();
  }

  void SetCluster(std::vector<ServerAddress> peers) {
    peers_ = std::move(peers);
  }

  void Start() override {
    WHIRL_LOG("Start node at server " << NetAddress());

    auto g = heap_.Use();
    // Copy peers!
    node_->SetCluster(peers_);
    node_->Start();
  }

  NodeId GetId() const {
    return config_.id;
  }

  // IActor

  const std::string& Name() const override {
    return name_;
  }

  bool IsRunnable() const override {
    if (paused_) {
      return false;
    }
    auto g = heap_.Use();
    return !events_.IsEmpty();
  }

  TimePoint NextStepTime() override {
    auto g = heap_.Use();
    return events_.NextEventTime();
  }

  void Step() override {
    auto g = heap_.Use();
    auto event = events_.TakeNext();
    event();
  }

  void Shutdown() override {
    Crash();
  }

 private:
  std::string MakeName(const NodeConfig& config) {
    return wheels::StringBuilder() << "Server-" << config.id;
  }

  NodeServices CreateNodeServices() {
    NodeServices services;

    auto executor = std::make_shared<EventQueueExecutor>(events_);
    auto time_service = std::make_shared<TimeService>(
        local_wall_time_clock_, local_monotonic_clock_, events_);

    services.threads = ThreadsRuntime{executor, time_service};
    services.time_service = time_service;

    services.storage_engine =
        std::make_shared<LocalStorageEngine>(local_storage_);
    services.local_storage = LocalStorage(services.storage_engine);

    services.rpc_server =
        TRPCServer(std::make_shared<RPCServer>(heap_, network_, executor));
    services.rpc_client =
        TRPCClient(std::make_shared<RPCClient>(heap_, network_));

    services.random = std::make_shared<RandomService>();
    services.uids = std::make_shared<UidGenerator>();
    services.true_time = std::make_shared<TrueTimeService>();

    services.logger = std::make_shared<LoggerProxy>();

    return services;
  }

  void Create() {
    local_monotonic_clock_.Reset();

    auto g = heap_.Use();
    auto services = CreateNodeServices();
    node_ = node_factory_->CreateNode(std::move(services), config_);
  }

  void Crash() {
    WHIRL_LOG("Crash server " << NetAddress());

    // Reset all client connections
    network_.Reset();

    WHIRL_LOG("Bytes allocated on process heap: " << heap_.BytesAllocated());
    {
      auto g = heap_.Use();
      events_.Clear();
      // Node is located on server's heap
      node_.release();
    }
    heap_.Reset();

    paused_ = false;
  }

 private:
  NodeConfig config_;
  INodeFactoryPtr node_factory_;
  std::string name_;

  ProcessNetwork network_;
  std::vector<ServerAddress> peers_;

  LocalWallTimeClock local_wall_time_clock_;
  LocalMonotonicClock local_monotonic_clock_;
  LocalBytesStorage local_storage_;

  // Node process

  mutable ProcessHeap heap_;
  EventQueue events_;
  bool paused_{false};  // TODO: better?

  INodePtr node_;

  Logger logger_{"Server"};
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl
