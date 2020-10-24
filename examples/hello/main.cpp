#include <whirl/node/node_base.hpp>
#include <whirl/node/logging.hpp>

// Simulation
#include <whirl/matrix/world/world.hpp>
#include <whirl/matrix/world/faults.hpp>
#include <whirl/matrix/world/global.hpp>

#include <await/fibers/sync/future.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/thread_like.hpp>

#include <cereal/types/string.hpp>

#include <wheels/support/time.hpp>

#include <chrono>
#include <cstdlib>

using namespace std::chrono_literals;

using namespace await::fibers;
using namespace whirl;

//////////////////////////////////////////////////////////////////////

class ChattyNode final: public NodeBase {
 public:
  ChattyNode(NodeServices services, NodeConfig config)
      : NodeBase(std::move(services), config) {
  }

 protected:
  void RegisterRPCMethods(TRPCServer& rpc_server) override {
    rpc_server.RegisterMethod("Hello", &ChattyNode::Ping, this);
  }

  std::string Ping(std::string who) {
    NODE_LOG("Ping from {}", who);
    return "Pong";
  }

  void MainThread() override {
    if (LocalStorage().Has("step")) {
      NODE_LOG("Start step: {}", LocalStorage().Load<int>("step"));
    }

    for (size_t i = 0; ; ++i) {
      NODE_LOG("Server {} makes step {}", Id(), i + 1);

      size_t peer_id = RandomNumber() % PeerCount();

      auto peer = PeerName(peer_id);
      NODE_LOG("Try to send Hello to {}", peer);

      auto status = Await(PeerChannel(peer_id).Call("Hello", MyName()).As<void>());
      if (status.IsOk()) {
        NODE_LOG("Peer {} responded with ack", peer);
      } else {
        NODE_LOG("Peer {}: RPC error", peer);
      }

      LocalStorage().Store<int>("progress", i);
      NODE_LOG("Save step: {}", i);

      Threads().Yield();
    }
  }
};

//////////////////////////////////////////////////////////////////////

class TestAdversary {
 public:
  TestAdversary(ThreadsRuntime& runtime)
     : runtime_(runtime) {
  }

  void Run() {
    for (size_t i = 0; i < ServerCount(); ++i) {
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
    auto& server = AccessFaultyServer(index);

    for (size_t i = 0; ; ++i) {

      if (GlobalNow() > 400) {
        runtime_.SleepFor(17);
        continue;
      }

      // Pause/resume server
      if (whirl::GlobalRandomNumber() % 17 == 0) {
        WHIRL_FMT_LOG("FAULT: Pause {}", GetServerName(index));
        server.Pause();
        runtime_.SleepFor(whirl::GlobalRandomNumber() % 20);
        WHIRL_FMT_LOG("FAULT: Resume {}", GetServerName(index));
        server.Resume();
        continue;
      }

      // Restart server
      if (whirl::GlobalRandomNumber() % 13 == 0) {
        WHIRL_FMT_LOG("FAULT: Reboot {}", GetServerName(index));
        server.Reboot();
      }

      // Adjust wall time clock
      if (whirl::GlobalRandomNumber() % 100 == 0) {
        WHIRL_FMT_LOG("FAULT: Adjust wall time at {}", GetServerName(index));
        server.AdjustWallClock();
      }

      // Just wait some time
      runtime_.SleepFor(5);
    }
  }

  void AbuseNetwork() {
    // Mess with network
  }

 private:
  ThreadsRuntime& runtime_;
  Logger logger_{"Adversary"};
};

void Adversary(ThreadsRuntime& runtime) {
  TestAdversary(runtime).Run();
}

//////////////////////////////////////////////////////////////////////

int main() {
  World world;
  auto node = MakeNode<ChattyNode>();

  world.AddServer(node);
  world.AddServer(node);
  world.AddServer(node);
  world.SetAdversary(Adversary);
  world.Start();

  wheels::StopWatch stop_watch;

  world.MakeSteps(1000);

  auto elapsed = stop_watch.Elapsed();

  world.Stop();

  std::cout << "Simulation completed, time = "
            << std::chrono::duration<double>(elapsed).count() << std::endl;

  return 0;
}
