#include <whirl/node/node_base.hpp>
#include <whirl/node/logging.hpp>

#include <whirl/rpc/use/service_base.hpp>

// Simulation
#include <whirl/matrix/world/world.hpp>
#include <whirl/matrix/client/client.hpp>

#include <await/fibers/sync/future.hpp>
#include <await/fibers/core/await.hpp>

#include <cereal/types/string.hpp>

#include <chrono>
#include <cstdlib>

using namespace await::fibers;
using namespace whirl;

//////////////////////////////////////////////////////////////////////

// Echo service

class EchoService : public rpc::RPCServiceBase<EchoService> {
 public:
  std::string Echo(std::string input) {
    return input;
  }

 protected:
  void RegisterRPCMethods() {
    RPC_REGISTER_METHOD(Echo);
  }
};

//////////////////////////////////////////////////////////////////////

// Echo server node

class EchoServerNode final: public NodeBase {
 public:
  EchoServerNode(NodeServices services, NodeConfig config)
      : NodeBase(std::move(services), config) {
  }

 protected:
  void RegisterRPCServices(const IRPCServerPtr& rpc_server) override {
    rpc_server->RegisterService(
        "Echo", std::make_shared<EchoService>());
  }
};

//////////////////////////////////////////////////////////////////////

class ClientNode final: public ClientBase {
 public:
  ClientNode(NodeServices services, NodeConfig config)
      : ClientBase(std::move(services), config) {
  }

 protected:
  void MainThread() override {
    while (true) {
      // Печатаем текущее системное время
      NODE_LOG("Local wall time: {}", WallTimeNow());

      // Получаем RPC канал до случайного узла кластера
      auto& channel = Channel();

      // Печатаем сетевой адрес узла, к которому полетит запрос
      NODE_LOG("Make RPC to server {}", channel.Peer());

      // Выполняем RPC
      // Имя метода - "Echo", аргумент - имя текущего клиента
      // Результат вызова - Future, она типизируется вызовом .As<std::string>()

      // Фьючу дожидаемся синхронно с помощью функции Await
      // Она распаковывает фьючу в Result<std::string>
      // См. <await/fibers/sync/future.hpp>
      auto result = Await(
          Channel().Call("Echo.Echo", MyName()).As<std::string>());

      if (result.IsOk()) {
        NODE_LOG("Echo response from {}: '{}'", channel.Peer(), *result);
      } else {
        NODE_LOG("Echo request failed");
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
  World world{17};

  // Cluster nodes
  auto node = MakeNode<EchoServerNode>();
  world.AddServers(3, node);

  // Clients
  auto client = MakeNode<ClientNode>();
  world.AddClient(client);

  world.Start();
  world.MakeSteps(256);
  world.Stop();

  return 0;
}
