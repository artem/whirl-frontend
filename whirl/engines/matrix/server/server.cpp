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
      transport_(net, config.hostname, heap_, scheduler_) {
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

  // Remove all network endpoints
  transport_.Reset();

  // Close open files
  filesystem_.Reset();

  // Drop scheduled tasks
  scheduler_.Reset();

  // 2) Reset process heap
  // WHIRL_LOG("Bytes allocated on process heap: " << heap_.BytesAllocated());
  {
    auto g = heap_.Use();
    runtime_ = nullptr;
    ReleaseFiberResourcesOnCrash(heap_);
  }
  heap_.Reset();

  // Clear stdout?

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

  scheduler_.Resume(GlobalNow());

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

  // Start node process

  runtime_ = MakeNodeRuntime();
  // Now runtime is accessible from node via GetRuntime()

  // TODO: this is ugly, we need abstractions for program/process
  auto node = node_factory_->CreateNode();
  StartNodeMain(node.get());
  MoveToHeap(std::move(node));

  state_ = State::Running;
}

void Server::StartNodeMain(INode* node) {
  await::fibers::Go([node]() {
    node->Start();
    }, runtime_->Executor());
}

bool Server::IsRunnable() const {
  if (state_ != State::Running) {
    return false;
  }
  return !scheduler_.IsEmpty();
}

TimePoint Server::NextStepTime() const {
  return scheduler_.NextTaskTime();

}

void Server::Step() {
  ITask* task = scheduler_.TakeNext();
  {
    auto g = heap_.Use();
    task->Run();
  }
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
  // Memory
  digest.Eat(heap_.BytesAllocated());
  // Fs
  digest.Combine(filesystem_.ComputeDigest());
  return digest.GetValue();
}

// Private

INodeRuntime* Server::MakeNodeRuntime() {
  NodeRuntime* runtime = new NodeRuntime();

  runtime->thread_pool.Init(scheduler_);

  runtime->time
      .Init(wall_clock_, monotonic_clock_, scheduler_);

  runtime->fs.Init(&filesystem_, runtime->time.Get());

  runtime->db.Init(runtime->fs.Get());

  static const net::Port kTransportPort = 42;
  runtime->transport.Init(transport_, kTransportPort);

  runtime->random.Init();
  runtime->guids.Init(config_.id);

  runtime->true_time.Init();

  runtime->config.Init(config_.id);

  runtime->discovery.Init();

  runtime->terminal.Init(stdout_);

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