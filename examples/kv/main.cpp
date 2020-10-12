#include <whirl/node/node_base.hpp>
#include <whirl/node/logging.hpp>

// Simulation
#include <whirl/matrix/world/world.hpp>
#include <whirl/matrix/client/client.hpp>
#include <whirl/matrix/history/printers/kv.hpp>

#include <await/fibers/sync/future.hpp>
#include <await/fibers/core/await.hpp>
#include <await/futures/combine/quorum.hpp>

#include <whirl/helpers/serialize.hpp>

#include <cereal/types/string.hpp>
#include <fmt/ostream.h>

#include <chrono>
#include <cstdlib>

using namespace std::chrono_literals;

using namespace await::fibers;
using namespace whirl;

using wheels::Result;
using wheels::Status;

//////////////////////////////////////////////////////////////////////

// string -> int
using Key = std::string;
using Value = int;

//////////////////////////////////////////////////////////////////////

// Реплики хранят версионированные значения

using Timestamp = size_t;

struct StampedValue {
  Value value;
  Timestamp ts;

  static StampedValue NoValue() {
    return {0, 0};
  }

  // Сериализация для локального хранилища и передачи по сети
  SERIALIZE(value, ts)
};

// Для логирования
std::ostream& operator<< (std::ostream& out, const StampedValue& v) {
  out << "{" << v.value << ", " << v.ts << "}";
  return out;
}

//////////////////////////////////////////////////////////////////////

// KV storage node

class KVNode final: public NodeBase {
 public:
  KVNode(NodeServices services, NodeConfig config)
      : NodeBase(std::move(services), config),
        kv_(StorageBackend(), "test") {
  }

 protected:
  void RegisterRPCMethods(rpc::TRPCServer& rpc_server) override {
    rpc_server.RegisterMethod("Set",
        [this](Key k, Value v) { Set(k, v); });

    rpc_server.RegisterMethod("Get",
        [this](Key k) { return Get(k); });

    rpc_server.RegisterMethod("Write",
        [this](Key k, StampedValue v) { Write(k, v); });

    rpc_server.RegisterMethod("Read",
        [this](Key k) { return Read(k); });
  }

  // RPC method handlers

  // Публичные операции - Set/Get

  void Set(Key k, Value v) {
    Timestamp write_ts = ChooseWriteTimestamp();
    NODE_LOG("Write timestamp: {}", write_ts);

    std::vector<Future<void>> writes;
    for (size_t i = 0; i < PeerCount(); ++i) {
      writes.push_back(
          PeerChannel(i).Call(
              "Write", k, StampedValue{v, write_ts}));
    }

    // Синхронно дожидаемся большинства подтверждений
    Await(Quorum(std::move(writes), Majority())).ExpectOk();
  }

  Value Get(Key k) {
    std::vector<Future<StampedValue>> reads;

    // Отправляем пирам команду Read(k)
    for (size_t i = 0; i < PeerCount(); ++i) {
      reads.push_back(
          PeerChannel(i).Call("Read", k));
    }

    // Собираем кворум большинства
    auto values = Await(Quorum(std::move(reads), Majority())).Value();

    for (size_t i = 0; i < values.size(); ++i) {
      NODE_LOG("{}-th value in read quorum: {}", i + 1, values[i]);
    }

    auto winner = FindNewestValue(values);
    return winner.value;
  }

  // Внутренние команды репликам

  void Write(Key k, StampedValue v) {
    std::optional<StampedValue> local = kv_.TryGet(k);

    if (!local.has_value()) {
      // Раньше не видели данный ключ
      Update(k, v);
    } else {
      // Если временная метка записи больше, чем локальная,
      // то обновляем значение в локальном хранилище
      if (v.ts > local->ts) {
        Update(k, v);
      }
    }
  }

  void Update(Key k, StampedValue v) {
    NODE_LOG("Write '{}' -> {}", k, v);
    kv_.Set(k, v);
  }

  StampedValue Read(Key k) {
    return kv_.GetOr(k, StampedValue::NoValue());
  }

  Timestamp ChooseWriteTimestamp() {
    // Локальные часы могут быть рассинхронизированы
    // Возмонжо стоит использовать сервис TrueTime?
    // См. TrueTime()
    return WallTimeNow();
  }

  // Выбираем самый свежий результат из кворумных чтений
  StampedValue FindNewestValue(const std::vector<StampedValue>& values) const {
    size_t winner = 0;
    for (size_t i = 1; i < values.size(); ++i) {
      if (values[i].ts > values[winner].ts) {
        winner = i;
      }
    }
    return values[winner];
  }

  // Размер кворума
  size_t Majority() const {
    return PeerCount() / 2 + 1;
  }

 private:
  // Локальное персистентное K/V хранилище
  // Ключи - строки, значения - StampedValue
  LocalKVStorage<StampedValue> kv_;
};

//////////////////////////////////////////////////////////////////////

class KVClient final: public ClientBase {
 public:
  KVClient(NodeServices services, NodeConfig config)
      : ClientBase(std::move(services), config) {
  }

 protected:
  void Set(Key k, Value v) {
    Await(Channel().Call("Set", k, v).As<void>()).ExpectOk();
  }

  Value Get(Key k) {
    return Await(Channel().Call("Get", k).As<Value>()).Value();
  }

  void MainThread() override {
    for (size_t i = 1; ; ++i) {
      // Печатаем текущее системное время
      NODE_LOG("Local wall time: {}", WallTimeNow());

      // Подкинем монетку
      if (RandomNumber() % 2 == 0) {
        // Запись случайного значения
        Value value = RandomNumber(1, 100);
        NODE_LOG("Execute Set({})", value);
        Set("test", value);
        NODE_LOG("Set completed");
      } else {
        // Чтение
        NODE_LOG("Execute Get(test)");
        Value result = Get("test");
        NODE_LOG("Get(test) -> {}", result);
      }

      Threads().SleepFor(RandomNumber(1, 100));
    }
  }
};

//////////////////////////////////////////////////////////////////////

int main() {
  World world{/*seed=*/18};

  // Cluster nodes
  auto node = MakeNode<KVNode>();
  world.AddServers(3, node);

  // Clients
  auto client = MakeNode<KVClient>();
  world.AddClient(client);
  world.AddClient(client);
  world.AddClient(client);

  world.Start();
  world.MakeSteps(500);
  world.Stop();

  const auto history = world.History();

  std::cout << std::endl << "History: " << std::endl;
  histories::PrintKVHistory<Key, Value>(history);

  return 0;
}
