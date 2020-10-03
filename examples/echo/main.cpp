#include <whirl/node/node_base.hpp>
#include <whirl/matrix/client/client.hpp>

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

// Echo server

class EchoServerNode final: public NodeBase {
 public:
  EchoServerNode(NodeServices services, NodeConfig config)
      : NodeBase(std::move(services), config) {
  }

 protected:
  void RegisterRPCMethods(TRPCServer& rpc_server) override {
    rpc_server.RegisterMethod("Echo",
                              [this](std::string data) { return Echo(data); });
  }

  // RPC method handlers

  std::string Echo(std::string data) {
    WHIRL_LOG("Echo request: '" << data << "'");
    return data ;
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
      WHIRL_LOG("Local wall time: " << WallTimeNow());

      // Получаем RPC канал до случайного узла кластера
      auto& channel = Channel();

      // Печатаем сетевой адрес узла, к которому полетит запрос
      WHIRL_LOG("Make RPC to server " << channel.Peer());

      // Выполняем RPC
      // Имя метода - "Echo", аргумент - имя текущего клиента
      // Результат вызова - Future, она типизируется вызовом .As<std::string>()

      // Фьючу дожидаемся синхронно с помощью функции Await
      // Она распаковывает фьючу в Result<std::string>
      // См. <await/fibers/sync/future.hpp>
      auto result = Await(
          Channel().Call("Echo", MyName()).As<std::string>());

      if (result.IsOk()) {
        WHIRL_LOG("Echo response from " << channel.Peer() << ": '" << *result << "'");
      } else {
        WHIRL_LOG("Echo request failed");
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
  world.AddServer(node);
  world.AddServer(node);
  world.AddServer(node);

  // Clients
  auto client = MakeNode<ClientNode>();
  world.AddClient(client);

  world.Start();
  world.MakeSteps(256);
  world.Stop();

  return 0;
}
