#include <whirl/engines/matrix/server/server.hpp>

#include <whirl/engines/matrix/memory/helpers.hpp>

#include <whirl/engines/matrix/process/crash.hpp>

#include <whirl/engines/matrix/world/global/actor.hpp>

// TODO
#include <whirl/engines/matrix/server/services/runtime.hpp>
#include <whirl/engines/matrix/server/services/locator.hpp>

#include <whirl/engines/matrix/helpers/digest.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

Server::Server(net::Network& net, ServerConfig config, INodeFactoryPtr factory)
    : config_(config),
      node_factory_(std::move(factory)),
      transport_(net, config.hostname, heap_) {
}

Server::~Server() {
  node_factory_.reset();
  WHEELS_VERIFY(state_ == State::Crashed, "Invalid state");
}

// INetServer

void Server::HandlePacket(const net::Packet& packet, net::Link* out) {
  transport_.HandlePacket(packet, out);
}

// IFaultyServer

bool Server::IsAlive() const {
  return state_ == State::Running || state_ == State::Paused;
}

void Server::Crash() {
  GlobalAllocatorGuard g;

  WHEELS_VERIFY(state_ != State::Crashed, "Server already crashed");

  WHIRL_LOG_INFO("Crash server {}", HostName());

  // 1) Remove all network endpoints
  transport_.Reset();

  // 2) Reset process heap
  // WHIRL_LOG("Bytes allocated on process heap: " << heap_.BytesAllocated());
  {
    auto g = heap_.Use();
    steps_ = nullptr;
    runtime_ = nullptr;
    ReleaseFiberResourcesOnCrash(heap_);
  }
  heap_.Reset();

  state_ = State::Crashed;
}

void Server::FastReboot() {
  GlobalAllocatorGuard g;

  Crash();
  Start();
}

void Server::Pause() {
  GlobalAllocatorGuard g;

  WHEELS_VERIFY(state_ != State::Paused, "Server already paused");
  state_ = State::Paused;
}

void Server::Resume() {
  GlobalAllocatorGuard g;

  WHEELS_VERIFY(state_ == State::Paused, "Server is not paused");

  auto now = GlobalNow();

  {
    auto g = heap_.Use();

    while (!steps_->IsEmpty() && steps_->NextStepTime() < now) {
      auto step = steps_->TakeNext();
      steps_->Add(now, std::move(step.action));
    }
  }

  state_ = State::Running;
}

void Server::AdjustWallClock() {
  GlobalAllocatorGuard g;
  wall_clock_.AdjustOffset();
}

// IActor

const std::string& Server::Name() const {
  return HostName();
}

void Server::Start() {
  WHEELS_VERIFY(state_ == State::Initial || state_ == State::Crashed,
                "Invalid state");

  monotonic_clock_.Reset();

  WHIRL_LOG_INFO("Start node at server {}", HostName());

  auto g = heap_.Use();

  steps_ = new StepQueue();

  // Start node process

  runtime_ = MakeNodeRuntime();
  // Now runtime is accessible from node via GetRuntime()

  // TODO: this is ugly, we need abstractions for program/process
  auto node = node_factory_->CreateNode();
  node->Start();
  MoveToHeap(std::move(node));

  state_ = State::Running;
}

bool Server::IsRunnable() const {
  if (state_ != State::Running) {
    return false;
  }
  // No [de]allocations here
  // auto g = heap_.Use();
  // WHEELS_VERIFY(steps_, "Step queue is not created");
  return !steps_->IsEmpty();
}

TimePoint Server::NextStepTime() const {
  // No [de]allocations here
  // auto g = heap_.Use();
  return steps_->NextStepTime();
}

void Server::Step() {
  auto g = heap_.Use();
  auto step = steps_->TakeNext();
  step();
}

void Server::Shutdown() {
  if (state_ != State::Crashed) {
    Crash();
  }
}

size_t Server::ComputeDigest() const {
  if (state_ == State::Crashed) {
    return 0;
  }

  DigestCalculator digest;
  digest.Eat(heap_.BytesAllocated());
  digest.Combine(db_.ComputeDigest());
  return digest.GetValue();
}

// Private

INodeRuntime* Server::MakeNodeRuntime() {
  NodeRuntime* runtime = new NodeRuntime();

  runtime->thread_pool.Init(*steps_);

  runtime->time
      .Init(wall_clock_, monotonic_clock_, *steps_);

  runtime->db
      .Init(db_, runtime->time.Get());

  static const net::Port kTransportPort = 42;
  runtime->transport.Init(transport_, kTransportPort);

  runtime->random.Init();
  runtime->guids.Init(config_.id);

  runtime->true_time.Init();

  runtime->config.Init(config_.id);

  runtime->discovery.Init();

  return new RuntimeLocator{runtime};
}

INodeRuntime& Server::GetNodeRuntime() {
  return *runtime_;
}

//////////////////////////////////////////////////////////////////////

Server& ThisServer() {
  Server* this_server = dynamic_cast<Server*>(ThisActor());
  WHEELS_VERIFY(this_server != nullptr, "Current actor is not a server");
  return *this_server;
}

}  // namespace whirl::matrix