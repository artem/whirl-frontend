#pragma once

#include <whirl/services/node_services.hpp>
#include <whirl/services/node.hpp>

#include <whirl/matrix/actor.hpp>
#include <whirl/matrix/clock.hpp>
#include <whirl/matrix/network.hpp>
#include <whirl/matrix/process_network.hpp>
#include <whirl/matrix/event_queue.hpp>
#include <whirl/matrix/local_clocks.hpp>
#include <whirl/matrix/process_heap.hpp>

// Services

#include <whirl/matrix/message_bus.hpp>
#include <whirl/matrix/local_storage.hpp>
#include <whirl/matrix/true_time.hpp>
#include <whirl/matrix/rpc.hpp>
#include <whirl/matrix/uid.hpp>
#include <whirl/matrix/random.hpp>
#include <whirl/matrix/time_service.hpp>
#include <whirl/services/rpc.hpp>

#include <whirl/matrix/log.hpp>

#include <await/fibers/core/api.hpp>
#include <await/futures/future.hpp>
#include <await/futures/promise.hpp>
#include <await/executors/executor.hpp>

#include <memory>

namespace whirl {

using namespace await::fibers;
using namespace await::futures;
using namespace await::executors;

using wheels::Unit;

//////////////////////////////////////////////////////////////////////

// Execution service impl

class EventQueueExecutor : public IExecutor {
 public:
  EventQueueExecutor(const WorldClock& clock, EventQueue& events)
      : world_clock_(clock), events_(events) {
  }

  void Execute(Task&& task) override {
    auto tp = ChooseTaskTimePoint();
    events_.Add(tp, std::move(task));
  }

 private:
  size_t ChooseTaskTimePoint() {
    return world_clock_.Now() + 1;
  }

 private:
  const WorldClock& world_clock_;
  EventQueue& events_;
};

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

class Server : public IActor {
 public:
  Server(const WorldClock& world_clock, Network& network, NodeConfig config,
         INodeFactoryPtr factory)
      : config_(config),
        node_factory_(std::move(factory)),
        name_(MakeName(config)),
        world_clock_(world_clock),
        network_(network, name_),
        local_clock_(world_clock),
        local_monotonic_clock_(world_clock_) {
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

    auto now = world_clock_.Now();
    HeapScope guard(&heap_);

    while (!events_.IsEmpty() && events_.NextEventTime() < now) {
      auto pending_event = events_.TakeNext();
      events_.Add(now, std::move(pending_event.action));
    }

    paused_ = false;
  }

  void SetCluster(std::vector<ServerAddress> peers) {
    peers_ = std::move(peers);
  }

  void Start() {
    WHIRL_LLOG("Start node at server " << NetAddress());

    HeapScope guard(&heap_);
    // Copy peers!
    node_->JoinCluster(peers_);
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
    HeapScope guard(&heap_);
    return !events_.IsEmpty();
  }

  TimePoint NextStepTime() override {
    HeapScope guard(&heap_);
    return events_.NextEventTime();
  }

  void MakeStep() override {
    HeapScope guard(&heap_);
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

    auto executor = std::make_shared<EventQueueExecutor>(world_clock_, events_);
    auto time_service = std::make_shared<TimeService>(
        world_clock_, local_clock_, local_monotonic_clock_, events_);

    services.runtime = Runtime{executor, time_service};
    services.time_service = time_service;

    services.storage_engine =
        std::make_shared<LocalKVStorageImpl>(local_storage_);
    services.local_storage = LocalStorage(services.storage_engine);

    /*
    services.message_bus_server =
        std::make_shared<MessageBusServer>(network_, heap_, executor);
    services.message_bus = std::make_shared<MessageBus>(network_);
    */

    services.rpc_server =
        TRPCServer(std::make_shared<RPCServer>(heap_, network_, executor));
    services.rpc_client =
        TRPCClient(std::make_shared<RPCClient>(heap_, network_));

    services.random = std::make_shared<RandomService>();
    services.uids_ = std::make_shared<UidGenerator>();
    services.true_time = std::make_shared<TrueTimeService>();

    return services;
  }

  void Create() {
    local_monotonic_clock_.Reset();

    HeapScope guard(&heap_);
    auto services = CreateNodeServices();
    node_ = node_factory_->CreateNode(services, config_);
  }

  void Crash() {
    WHIRL_LLOG("Crash server " << NetAddress());

    // Reset all client connections
    network_.Reset();

    WHIRL_LLOG("Bytes allocated on process heap: " << heap_.BytesAllocated());
    {
      HeapScope guard(&heap_);
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

  const WorldClock& world_clock_;
  ProcessNetwork network_;
  std::vector<ServerAddress> peers_;

  LocalWallTimeClock local_clock_;
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
