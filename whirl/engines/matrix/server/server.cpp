#include <whirl/engines/matrix/server/server.hpp>

#include <whirl/engines/matrix/memory/helpers.hpp>

#include <whirl/engines/matrix/world/global/actor.hpp>
#include <whirl/engines/matrix/world/global/log.hpp>

#include <whirl/engines/matrix/process/trampoline.hpp>

#include <whirl/engines/matrix/server/runtime/runtime.hpp>

#include <whirl/engines/matrix/helpers/digest.hpp>

#include <timber/log.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

Server::Server(net::Network& net, ServerConfig config,
               node::ProgramMain program)
    : config_(config),
      program_(program),
      transport_(net, config.hostname, heap_, scheduler_),
      logger_("Server", GetLogBackend()) {
}

Server::~Server() {
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

  LOG_INFO("Crash server {}", HostName());

  // Remove all network endpoints
  transport_.Reset();
  // Close opened files
  filesystem_.Reset();
  // Drop scheduled tasks
  scheduler_.Reset();
  // 2) Clean memory
  heap_.Reset();

  runtime_ = nullptr;

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

node::fs::FileList Server::ListFiles(std::string_view prefix) {
  node::fs::FileList listed;

  auto iter = filesystem_.ListAllFiles();
  while (iter.IsValid()) {
    if ((*iter).starts_with(prefix)) {
      listed.push_back(*iter);
    }
  }

  return listed;
}

void Server::CorruptFile(const node::fs::Path& file_path) {
  GlobalAllocatorGuard g;
  filesystem_.Corrupt(file_path);
}

// IActor

const std::string& Server::Name() const {
  return HostName();
}

void Server::Start() {
  WHEELS_VERIFY(state_ == State::Initial || state_ == State::Crashed,
                "Invalid state");

  monotonic_clock_.Reset();

  LOG_INFO("Starting process");
  StartProcess();

  state_ = State::Running;
}

void Server::StartProcess() {
  auto guard = heap_.Use();

  // Prepare runtime
  runtime_ = MakeNodeRuntime();

  // Run user program
  process::Schedule(scheduler_, GlobalNow(), [this]() {
    process::MainTrampoline(program_);
  });
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
  process::ITask* task = scheduler_.TakeNext();
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

node::IRuntime* Server::MakeNodeRuntime() {
  NodeRuntime* runtime = new NodeRuntime();

  runtime->thread_pool.Init(scheduler_);
  runtime->fibers.Init();

  runtime->time.Init(wall_clock_, monotonic_clock_, scheduler_);

  runtime->fs.Init(&filesystem_, runtime->time.Get());

  runtime->db.Init(runtime->fs.Get());

  static const net::Port kTransportPort = 42;
  runtime->transport.Init(transport_, kTransportPort);

  runtime->random.Init();
  runtime->guids.Init(config_.id);

  runtime->true_time.Init();

  runtime->config.Init(config_.id, config_.pool);

  runtime->discovery.Init();

  runtime->terminal.Init(stdout_);

  return runtime;
}

node::IRuntime& Server::GetNodeRuntime() {
  return *runtime_;
}

//////////////////////////////////////////////////////////////////////

Server& ThisServer() {
  Server* this_server = dynamic_cast<Server*>(ThisActor());
  WHEELS_VERIFY(this_server != nullptr, "Current actor is not a server");
  return *this_server;
}

}  // namespace whirl::matrix