#include <whirl/node/program/util.hpp>
#include <whirl/node/runtime/shortcuts.hpp>

#include <commute/rpc/service_base.hpp>
#include <commute/rpc/call.hpp>

#include <timber/log.hpp>

#include <muesli/serializable.hpp>
#include <cereal/types/string.hpp>

// Simulation
#include <whirl/engines/matrix/world/world.hpp>
#include <whirl/engines/matrix/client/main.hpp>
#include <whirl/engines/matrix/client/rpc.hpp>
#include <whirl/engines/matrix/test/event_log.hpp>

#include <await/fibers/core/api.hpp>
#include <await/fibers/sync/future.hpp>

#include <chrono>
#include <cstdlib>
#include <cassert>

using namespace await::fibers;
using await::futures::Future;
using namespace whirl;

//////////////////////////////////////////////////////////////////////

// Echo service

namespace proto {

struct Echo {
  struct Request {
    std::string data;

    MUESLI_SERIALIZABLE(data);
  };

  struct Response {
    std::string data;

    MUESLI_SERIALIZABLE(data);
  };
};

}  // namespace proto

class EchoService : public commute::rpc::ServiceBase<EchoService> {
 public:
  EchoService()
    : logger_("EchoService", node::rt::LoggerBackend()) {

  }

  proto::Echo::Response Echo(proto::Echo::Request req) {
    // Каждый обработчик – отдельный файбер
    assert(await::fibers::AmIFiber());

    LOG_INFO("Echo({})", req.data);
    return {req.data};
  }

 protected:
  void RegisterMethods() override {
    COMM_RPC_REGISTER_METHOD(Echo);
  }

 private:
  timber::Logger logger_;
};

//////////////////////////////////////////////////////////////////////

// Echo server node

void EchoNode() {
  node::main::Prologue();

  auto rpc_server = node::rt::MakeRpcServer(/*port=*/42);

  rpc_server->RegisterService("Echo", std::make_shared<EchoService>());

  rpc_server->Start();

  node::main::BlockForever();
}

//////////////////////////////////////////////////////////////////////

[[noreturn]] void EchoClient() {
  matrix::client::Prologue();

  auto channel = matrix::client::MakeRpcChannel(/*pool_name=*/"echo", 42);

  timber::Logger logger_{"Client", node::rt::LoggerBackend()};

  while (true) {
    // Печатаем локальное время
    LOG_INFO("I am {}", node::rt::HostName());
    LOG_INFO("Local wall time: {}", node::rt::WallTimeNow().ToJiffies());

    // Выполняем RPC - вызываем метод "Echo" у сервиса "Echo"
    // Результат вызова - Future, она типизируется вызовом .As<std::string>()

    // Фьючу дожидаемся синхронно с помощью функции Await
    // Она распаковывает фьючу в Result<std::string>
    // См. <await/fibers/sync/future.hpp>

    Future<proto::Echo::Response> future =
        commute::rpc::Call("Echo.Echo")  //
            .Args(proto::Echo::Request{"Hello"})
            .Via(channel);

    auto result = Await(std::move(future));

    if (result.IsOk()) {
      LOG_INFO("Echo response: '{}'", result->data);
    } else {
      LOG_INFO("Echo request failed: {}",
                     result.GetError().GetErrorCode().message());
    }

    // SleepFor – приостановить текущий файбер (не поток!) на заданное время
    // RandomNumber(lo, hi) - генерация случайного числа
    node::rt::SleepFor(node::rt::RandomNumber(1, 100));
  }
}

//////////////////////////////////////////////////////////////////////

int main() {
  static const size_t kSeed = 17;

  matrix::World world{kSeed};

  world.MakePool("echo", EchoNode)
      .Size(3)
      .NameTemplate("Server");

  // Clients
  world.AddClient(EchoClient);

  world.Start();
  world.MakeSteps(256);
  size_t digest = world.Stop();

  std::cout << "Seed: " << kSeed << ", digest: " << digest
            << ", time: " << world.TimeElapsed()
            << ", steps: " << world.StepCount() << std::endl;

  std::cout << "Simulation log: " << std::endl;
  matrix::WriteTextLog(world.EventLog(), std::cout);
  std::cout << std::endl;

  return 0;
}
