#pragma once

#include <whirl/runtime/runtime.hpp>
#include <whirl/program/program.hpp>

#include <whirl/engines/matrix/world/actor.hpp>
#include <whirl/engines/matrix/fault/server.hpp>

#include <whirl/engines/matrix/server/config.hpp>
#include <whirl/engines/matrix/server/stdout.hpp>

#include <whirl/engines/matrix/clocks/monotonic.hpp>
#include <whirl/engines/matrix/clocks/wall.hpp>

#include <whirl/engines/matrix/fs/fs.hpp>

#include <whirl/engines/matrix/network/server.hpp>
#include <whirl/engines/matrix/network/network.hpp>
#include <whirl/engines/matrix/network/transport.hpp>

#include <whirl/engines/matrix/process/heap.hpp>
#include <whirl/engines/matrix/process/scheduler.hpp>

#include <whirl/logger/log.hpp>

namespace whirl::matrix {

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
  Server(net::Network& network, ServerConfig config, node::Program program);

  // Non-copyable
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  ~Server();

  // INetServer

  const std::string& HostName() const override {
    return config_.hostname;
  }

  net::ZoneId Zone() const override {
    return 0;  // TODO
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

  // Simulation

  std::vector<std::string> GetStdout() const {
    return stdout_.lines;
  }

  size_t ComputeDigest() const;

  node::IRuntime& GetNodeRuntime();

 private:
  node::IRuntime* MakeNodeRuntime();
  void StartProgram();

 private:
  State state_{State::Initial};

  ServerConfig config_;
  node::Program program_;

  // Hardware
  WallClock wall_clock_;
  MonotonicClock monotonic_clock_;

  // Operating system
  TaskScheduler scheduler_;
  fs::FileSystem filesystem_;
  mutable ProcessHeap heap_;
  net::Transport transport_;

  Stdout stdout_;

  node::IRuntime* runtime_{nullptr};

  Logger logger_{"Server"};
};

//////////////////////////////////////////////////////////////////////

Server& ThisServer();

}  // namespace whirl::matrix
