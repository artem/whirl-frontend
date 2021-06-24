#include <whirl/node/node_base.hpp>

#include <whirl/logger/log.hpp>

#include <whirl/rpc/service_base.hpp>
#include <whirl/rpc/call.hpp>

// Simulation
#include <whirl/engines/matrix/world/world.hpp>
#include <whirl/engines/matrix/client/client.hpp>

#include <await/fibers/sync/future.hpp>
#include <await/fibers/core/await.hpp>

#include <cereal/types/string.hpp>

#include <chrono>
#include <cstdlib>

using namespace await::fibers;
using namespace whirl::time_literals;
using namespace whirl;

//////////////////////////////////////////////////////////////////////

// Echo service

namespace proto {

struct Echo {
  struct Request {
    std::string data;

    WHIRL_SERIALIZE(data);
  };

  struct Response {
    std::string data;

    WHIRL_SERIALIZE(data);
  };
};

}  // namespace proto

class EchoService : public rpc::ServiceBase<EchoService> {
 public:
  proto::Echo::Response Echo(proto::Echo::Request req) {
    WHIRL_LOG_INFO("Echo({})", req.data);
    return {req.data};
  }

 protected:
  void RegisterRPCMethods() override {
    RPC_REGISTER_METHOD(Echo);
  }

 private:
  Logger logger_{"EchoService"};
};

//////////////////////////////////////////////////////////////////////

// Echo server node

class EchoNode final: public NodeBase {
 public:
  EchoNode(NodeServices services)
      : NodeBase(std::move(services)) {
  }

 protected:
  void RegisterRPCServices(const rpc::IServerPtr& rpc_server) override {
    rpc_server->RegisterService(
        "Echo", std::make_shared<EchoService>());
  }
};

//////////////////////////////////////////////////////////////////////

class ClientNode final: public matrix::ClientBase {
 public:
  ClientNode(NodeServices services)
      : ClientBase(std::move(services)) {
  }

 protected:
  void MainThread() override {
    while (true) {
      // Печатаем текущее системное время
      WHIRL_LOG_INFO("Local wall time: {}", WallTimeNow());

      // Выполняем RPC - вызываем метод "Echo" у сервиса "Echo"
      // Результат вызова - Future, она типизируется вызовом .As<std::string>()

      // Фьючу дожидаемся синхронно с помощью функции Await
      // Она распаковывает фьючу в Result<std::string>
      // См. <await/fibers/sync/future.hpp>

      /*
      auto result = Await(
          rpc::Call("Echo.Echo", proto::Echo::Request{"Hello!"})
            .Via(Channel())
            .As<proto::Echo::Response>());
      */

      Future<proto::Echo::Response> future = rpc::Call("Echo.Echo", proto::Echo::Request{"Hello"}).Via(Channel());
      auto result = Await(WithTimeout(std::move(future), 256_jiffies));

      if (result.IsOk()) {
        WHIRL_LOG_INFO("Echo response: '{}'", result->data);
      } else {
        WHIRL_LOG_INFO("Echo request failed: {}", result.GetError().GetErrorCode().message());
      }

      // Threads() - рантайм, с помощью которого можно запускать новые потоки
      // или работать с текущим потоком
      // RandomNumber(lo, hi) - генерация случайного числа
      Threads().SleepFor(RandomNumber(1, 100));
    }
  }
};

//////////////////////////////////////////////////////////////////////

int main() {
  static const size_t kSeed = 17;

  matrix::World world{kSeed};

  // Cluster nodes
  auto node = MakeNode<EchoNode>();
  world.AddServers(3, node);

  // Clients
  auto client = MakeNode<ClientNode>();
  world.AddClient(client);

  world.Start();
  world.MakeSteps(256);
  size_t digest = world.Stop();

  std::cout
      << "Seed: " << kSeed
      << ", digest: " << digest
      << ", time: " << world.TimeElapsed()
      << ", steps: " << world.StepCount()
      << std::endl;

  std::cout
      << "Simulation log: " << std::endl
      << world.TextLog()
      << std::endl;

  return 0;
}
