#include <whirl/node/node_base.hpp>
#include <whirl/matrix/client/client.hpp>

// Simulation
#include <whirl/matrix/server/server.hpp>
#include <whirl/matrix/world/world.hpp>
#include <whirl/matrix/world/global.hpp>

#include <await/fibers/sync/future.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/thread_like.hpp>

#include <await/futures/combine/quorum.hpp>

#include <wheels/support/random.hpp>
#include <wheels/support/string_builder.hpp>
#include <wheels/support/time.hpp>

#include <whirl/helpers/serialize.hpp>

#include <cereal/types/string.hpp>

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

  std::string ToString() const {
    return wheels::StringBuilder() << "{" << value << ", ts = " << ts << "}";
  }

  static StampedValue NoValue() {
    return {0, 0};
  }

  // Сериализация для локального хранилища и передачи по сети
  SERIALIZE(value, ts)
};

//////////////////////////////////////////////////////////////////////

// KV storage node

class KVNode final: public NodeBase {
 public:
  KVNode(NodeServices services, NodeConfig config)
      : NodeBase(std::move(services), config),
        kv_(StorageEngine(), "test") {
  }

 protected:
  void RegisterRPCMethods(TRPCServer& rpc_server) override {
    rpc_server.RegisterMethod("Set",
        [this](Key k, Value v) { Set(k, v); });

    rpc_server.RegisterMethod("Get",
        [this](Key k) { return Get(k); });

    rpc_server.RegisterMethod("Write",
        [this](Key k, StampedValue v) { return Write(k, v); });

    rpc_server.RegisterMethod("Read",
        [this](Key k) { return Read(k); });
  }

  // RPC method handlers

  // Публичные операции - Set/Get

  void Set(Key k, Value v) {
    Timestamp write_ts = ChooseWriteTimestamp();
    WHIRL_LOG("Write timestamp: " << write_ts);

    std::vector<Future<void>> writes;
    for (size_t i = 0; i < PeerCount(); ++i) {
      writes.push_back(
          PeerChannel(i).Call(
              "Write", k, StampedValue{v, write_ts}).As<void>());
    }

    // Синхронно дожидаемся большинства подтверждений
    Await(Quorum(std::move(writes), Majority())).ExpectOk();
  }

  Value Get(Key k) {
    std::vector<Future<StampedValue>> reads;

    // Отправляем пирам команду Read(k)
    for (size_t i = 0; i < PeerCount(); ++i) {
      reads.push_back(
          PeerChannel(i).Call(
              "Read", k).As<StampedValue>());
    }

    // Собираем кворум большинства
    auto values = *Await(Quorum(std::move(reads), Majority()));

    for (size_t i = 0; i < values.size(); ++i) {
      WHIRL_LOG((i + 1) << "-th value in Read quorum: " << values[i].ToString());
    }

    auto winner = FindNewestValue(values);
    return winner.value;
  }

  // Внутренние команды репликам

  void Write(Key k, StampedValue v) {
    if (!kv_.Has(k)) {
      kv_.Set(k, v);
    } else {
      // Локальный таймстемп записи
      Timestamp local_ts = kv_.Get(k).ts;
      if (v.ts > local_ts) {
        kv_.Set(k, v);
      }
    }
  }

  StampedValue Read(Key k) {
    if (kv_.Has(k)) {
      return kv_.Get(k);
    } else {
      return StampedValue::NoValue();
    }
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
  void Write(Key k, Value v) {
    Await(Channel().Call("Set", k, v).As<void>()).ExpectOk();
  }

  Value Read(Key k) {
    return Await(Channel().Call("Get", k).As<Value>()).Value();
  }

  void MainThread() override {
    for (size_t i = 1; ; ++i) {
      // Печатаем текущее системное время
      WHIRL_LOG("Local wall time: " << WallTimeNow());

      WHIRL_LOG("Execute Set(" << "test" << ", " << i << ")");
      Write("test", i);
      WHIRL_LOG("Set completed");

      Threads().SleepFor(RandomNumber(1, 100));

      WHIRL_LOG("Execute Get(test)");
      Value result = Read("test");

      WHIRL_LOG("Get(test) -> " << result << ", expected: " << i);
    }
  }
};

//////////////////////////////////////////////////////////////////////

int main() {
  World world{17};

  // Cluster nodes
  auto node = MakeNode<KVNode>();
  world.AddServer(node);
  world.AddServer(node);
  world.AddServer(node);

  // Clients
  auto client = MakeNode<KVClient>();
  world.AddClient(client);
  world.AddClient(client);

  world.Start();
  world.MakeSteps(256);
  world.Stop();

  return 0;
}
