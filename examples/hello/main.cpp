#include <whirl/node/node_base.hpp>

// Simulation
#include <whirl/matrix/server/server.hpp>
#include <whirl/matrix/world/world.hpp>
#include <whirl/matrix/world/global.hpp>

#include <await/fibers/sync/future.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/thread_like.hpp>

#include <wheels/support/random.hpp>
#include <wheels/support/string_builder.hpp>
#include <wheels/support/time.hpp>

#include <cereal/types/string.hpp>

#include <chrono>
#include <cstdlib>

using namespace std::chrono_literals;

using namespace await::fibers;
using namespace whirl;

using wheels::Result;
using wheels::Status;

//////////////////////////////////////////////////////////////////////

class TestNode final: public NodeBase {
 public:
  TestNode(NodeServices services, NodeConfig config)
      : NodeBase(std::move(services), config) {
  }

 protected:
  void RegisterRPCMethods(TRPCServer& rpc_server) override {
    rpc_server.RegisterMethod("Hello",
        [this](std::string who) { Hello(who); });
  }

  void Hello(std::string who) {
    WHIRL_LOG("Hello from " << who);
  }

  void MainThread() override {
    if (LocalStorage().Has("progress")) {
      WHIRL_LOG("Start progress: " << LocalStorage().Load<int>("progress"));
    }

    for (size_t i = 0; ; ++i) {
      WHIRL_LOG("Server " << Id() << " makes step " << i + 1);

      size_t peer_id = RandomNumber() % PeerCount();

      auto peer = PeerName(peer_id);
      WHIRL_LOG("Try to send Hello to " << peer);

      auto status = Await(PeerChannel(peer_id).Call("Hello", MyName()).As<void>());
      if (status.IsOk()) {
        WHIRL_LOG("Peer " << peer << " responded with ack");
      } else {
        WHIRL_LOG("Peer " << peer << ": RPC error");
      }

      LocalStorage().Store<int>("progress", i);
      WHIRL_LOG("Save progress to " << i);

      Yield();
    }
  }
};

//////////////////////////////////////////////////////////////////////

class TestAdversary {
 public:
  TestAdversary(ThreadsRuntime& runtime, WorldView world)
     : runtime_(runtime), world_(world) {
  }

  void Run() {
    for (size_t i = 0; i < world_.servers.size(); ++i) {
      runtime_.Spawn([this, i]() { AbuseServer(i); });
    }
    runtime_.Spawn([this]() { AbuseNetwork(); });
    RunForever();
  }

 private:
  void RunForever() {
    runtime_.SleepFor(123456789);  // Just long enough
  }

  void AbuseServer(size_t index) {
    Server& target = world_.servers[index];

    for (size_t i = 0; ; ++i) {
      // Pause server

      if (whirl::GlobalRandomNumber() % 17 == 0) {
        WHIRL_LOG("FAULT: Pause " << target.Name());
        target.Pause();
        runtime_.SleepFor(whirl::GlobalRandomNumber() % 20);
        WHIRL_LOG("FAULT: Resume " << target.Name());
        target.Resume();
        continue;
      }

      // Restart server
      if (whirl::GlobalRandomNumber() % 13 == 0) {
        WHIRL_LOG("FAULT: Reboot " << target.Name());
        target.Reboot();
      }

      // Adjust server clock?
      // TODO

      // Just wait some time
      runtime_.SleepFor(1);
    }
  }

  void AbuseNetwork() {
    // Mess with world_.network
  }

 private:
  ThreadsRuntime& runtime_;
  WorldView world_;
  Logger logger_{"Adversary"};
};

void RunAdversary(ThreadsRuntime& runtime, WorldView world) {
  TestAdversary(runtime, world).Run();
}

//////////////////////////////////////////////////////////////////////

int main() {
  World world;
  auto node = MakeNode<TestNode>();

  world.AddServer(node);
  world.AddServer(node);
  world.AddServer(node);
  world.SetAdversary(RunAdversary);
  world.Start();

  wheels::StopWatch stop_watch;

  world.MakeSteps(256);

  auto elapsed = stop_watch.Elapsed();

  world.Stop();

  std::cout << "Simulation completed, time = "
            << std::chrono::duration<double>(elapsed).count() << std::endl;

  return 0;
}
