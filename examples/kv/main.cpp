#include <whirl/node/node_base.hpp>
#include <whirl/node/local_storage.hpp>
#include <whirl/logger/log.hpp>
#include <whirl/rpc/service_base.hpp>
#include <whirl/rpc/call.hpp>
#include <whirl/cereal/serializable.hpp>

// Simulation
#include <whirl/engines/matrix/world/world.hpp>
#include <whirl/engines/matrix/client/client.hpp>
#include <whirl/engines/matrix/world/global/vars.hpp>
#include <whirl/engines/matrix/test/random.hpp>
#include <whirl/engines/matrix/memory/new.hpp>
#include <whirl/engines/matrix/test/main.hpp>
#include <whirl/engines/matrix/test/event_log.hpp>

#include <whirl/history/printers/kv.hpp>
#include <whirl/history/checker/check.hpp>
#include <whirl/history/models/kv.hpp>

#include <await/fibers/core/id.hpp>
#include <whirl/rpc/id.hpp>

// Concurrency
#include <await/fibers/sync/future.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/mutex.hpp>
#include <await/futures/combine/quorum.hpp>

// Support std::string serialization
#include <cereal/types/string.hpp>

#include <fmt/ostream.h>

#include <random>
#include <algorithm>

using wheels::Result;
using namespace await::fibers;
using namespace whirl;
using namespace whirl::time_literals;

//////////////////////////////////////////////////////////////////////

// string -> int
using Key = std::string;
using Value = int32_t;

//////////////////////////////////////////////////////////////////////

// Replicas store versioned (stamped) values

using Timestamp = size_t;

struct StampedValue {
  Value value;
  Timestamp ts;

  static StampedValue NoValue() {
    return {0, 0};
  }

  // Serialization support for local storage and RPC
  WHIRL_SERIALIZE(value, ts)
};

// For logging
std::ostream& operator<<(std::ostream& out, const StampedValue& stamped_value) {
  out << "{" << stamped_value.value << ", ts: " << stamped_value.ts << "}";
  return out;
}

//////////////////////////////////////////////////////////////////////

// KV storage / bunch of atomic R/W registers

// RPC services / algorithm roles

class Coordinator : public rpc::ServiceBase<Coordinator>,
                    public PeerBase {
 public:
  Coordinator() {
  }

  void RegisterRPCMethods() override {
    RPC_REGISTER_METHOD(Set);
    RPC_REGISTER_METHOD(Get);
  };

  // RPC handlers

  void Set(Key key, Value value) {
    Timestamp write_ts = ChooseWriteTimestamp();
    WHIRL_LOG_INFO("Write timestamp: {}", write_ts);

    std::vector<Future<void>> writes;
    // TODO: iterate over configuration
    for (size_t i = 0; i < PeerCount(); ++i) {
      writes.push_back(
          rpc::Call("Replica.LocalWrite", key, StampedValue{value, write_ts})
              .Via(PeerChannel(i))
              .StopAdvice(await::fibers::self::LifetimeToken()));
    }

    // Await acks from majority of replicas
    Await(Quorum(std::move(writes), /*threshold=*/Majority())).ThrowIfError();
  }

  Value Get(Key key) {
    std::vector<Future<StampedValue>> reads;

    // Broadcast LocalRead request to replicas
    for (size_t i = 0; i < PeerCount(); ++i) {
      reads.push_back(
          rpc::Call("Replica.LocalRead", key)
              .Via(PeerChannel(i)));
    }

    // Await responses from majority of replicas

    // 1) Combine futures from read RPC-s to single quorum future
    Future<std::vector<StampedValue>> quorum_reads =
        Quorum(std::move(reads), /*threshold=*/Majority());
    // 2) Block current fiber until quorum collected
    Result<std::vector<StampedValue>> results = Await(std::move(quorum_reads));
    // 3) Unpack vector or throw error
    auto values = results.ValueOrThrow();

    // Or combine all steps into:
    // auto values = Await(Quorum(std::move(reads), Majority())).ValueOrThrow()

    for (size_t i = 0; i < values.size(); ++i) {
      WHIRL_LOG_INFO("{}-th value in read quorum: {}", i + 1, values[i]);
    }

    auto most_recent = FindMostRecentValue(values);
    return most_recent.value;
  }

 private:
  Timestamp ChooseWriteTimestamp() const {
    // Local wall clock may be out of sync with other replicas
    // Use TrueTime (TrueTime() method)
    return WallTimeNow();
  }

  // Find value with largest timestamp
  StampedValue FindMostRecentValue(
      const std::vector<StampedValue>& values) const {
    return *std::max_element(
        values.begin(), values.end(),
        [](const StampedValue& lhs, const StampedValue& rhs) {
          return lhs.ts < rhs.ts;
        });
  }

  // Quorum size
  size_t Majority() const {
    return PeerCount() / 2 + 1;
  }

 private:
  Logger logger_{"KVNode.Coordinator"};
};

class StorageReplica : public rpc::ServiceBase<StorageReplica>,
                       public NodeMethodsBase {
 public:
  StorageReplica()
    : kv_(StorageBackend(), "kv") {
  }

  void RegisterRPCMethods() override {
    RPC_REGISTER_METHOD(LocalWrite);
    RPC_REGISTER_METHOD(LocalRead);
  };

  // RPC handlers

  void LocalWrite(Key key, StampedValue stamped_value) {
    std::lock_guard guard(mutex_);

    std::optional<StampedValue> local = kv_.TryGet(key);

    if (!local.has_value()) {
      // First write for this key
      LocalUpdate(key, stamped_value);
    } else {
      // Write timestamp > timestamp of locally stored value
      if (stamped_value.ts > local->ts) {
        LocalUpdate(key, stamped_value);
      }
    }
  }

  void LocalUpdate(Key key, StampedValue stamped_value) {
    WHIRL_LOG_INFO("Write '{}' -> {}", key, stamped_value);
    kv_.Set(key, stamped_value);
  }

  StampedValue LocalRead(Key key) {
    std::lock_guard guard(mutex_);  // Blocks fiber, not thread!
    return kv_.GetOr(key, StampedValue::NoValue());
  }

 private:
  // Local persistent K/V storage
  // strings -> StampedValues
  LocalKVStorage<StampedValue> kv_;
  // Fiber-aware mutex
  // Guards access to kv_ from RPC handlers
  await::fibers::Mutex mutex_;

  Logger logger_{"KVNode.Replica"};
};

class KVNode final : public NodeBase {
 public:
  KVNode() {
  }

 protected:
  void RegisterRPCServices(const rpc::IServerPtr& rpc_server) override {
    rpc_server->RegisterService("KV", MakeCoordinatorService());
    rpc_server->RegisterService("Replica", MakeReplicaService());
  }

  void MainThread() override {
    // Do nothing
  }

 private:
  rpc::IServicePtr MakeCoordinatorService() {
    return std::make_shared<Coordinator>();
  }

  rpc::IServicePtr MakeReplicaService() {
    return std::make_shared<StorageReplica>();
  }
};

//////////////////////////////////////////////////////////////////////

class KVBlockingStub {
 public:
  KVBlockingStub(rpc::IChannelPtr channel) : channel_(channel) {
  }

  void Set(Key k, Value v) {
    Await(rpc::Call("KV.Set", k, v).Via(channel_).As<void>()).ThrowIfError();
  }

  Value Get(Key k) {
    return Await(rpc::Call("KV.Get", k).Via(channel_).As<Value>()).ValueOrThrow();
  }

 private:
  rpc::IChannelPtr channel_;
};

//////////////////////////////////////////////////////////////////////

static const std::vector<std::string> kKeys({"a", "b", "c"});

//////////////////////////////////////////////////////////////////////

class KVClient final : public matrix::ClientBase {
 public:
  KVClient() {
  }

 protected:
  void MainThread() override {
    KVBlockingStub kv_store{Channel()};

    for (size_t i = 1;; ++i) {
      if (Either()) {
        Key key = ChooseKey();
        Value value = RandomNumber(1, 100);
        WHIRL_LOG_INFO("Execute Set({}, {})", key, value);
        kv_store.Set(key, value);
        WHIRL_LOG_INFO("Set completed");
      } else {
        Key key = ChooseKey();
        WHIRL_LOG_INFO("Execute Get({})", key);
        [[maybe_unused]] Value result = kv_store.Get(key);
        WHIRL_LOG_INFO("Get({}) -> {}", key, result);
      }

      matrix::GlobalCounter("requests").Increment();

      // Random pause
      SleepFor(RandomNumber(1, 100));
    }
  }

 private:
  const std::string& ChooseKey() const {
    return kKeys.at(RandomNumber(matrix::GetGlobal<size_t>("keys")));
  }

 private:
  Logger logger_{"KVClient"};
};

//////////////////////////////////////////////////////////////////////

// Sequential specification for KV storage
// Used by linearizability checker
using KVStoreModel = histories::KVStoreModel<Key, Value>;

//////////////////////////////////////////////////////////////////////

void FailTest() {
  std::cout << "(ﾉಥ益ಥ）ﾉ ┻━┻" << std::endl;
  std::cout.flush();
  std::exit(1);
}

//////////////////////////////////////////////////////////////////////

// Seed -> simulation digest
size_t RunSimulation(size_t seed) {
  static const size_t kTimeLimit = 10000;
  static const size_t kRequestsThreshold = 7;

  matrix::Random random{seed};

  // Randomize simulation parameters
  const size_t replicas = random.Get(3, 5);
  const size_t clients = random.Get(2, 3);
  const size_t keys = random.Get(1, 2);

  std::cout << "Simulation seed: " << seed << std::endl;

  std::cout << "Parameters: "
            << "replicas = " << replicas << ", "
            << "clients = " << clients << ", "
            << "keys = " << keys << std::endl;

  // Reset RPC and fiber ids
  await::fibers::ResetIds();
  whirl::rpc::ResetIds();

  matrix::World world{seed};

  // Cluster nodes
  auto node = MakeNode<KVNode>();
  world.AddServers(replicas, node);

  // Clients
  auto client = MakeNode<KVClient>();
  world.AddClients(clients, client);

  // Globals
  world.SetGlobal("keys", keys);
  world.InitCounter("requests", 0);

  // Run simulation
  world.Start();
  while (world.GetCounter("requests") < kRequestsThreshold &&
         world.TimeElapsed() < kTimeLimit) {
    if (!world.Step()) {
      break;  // Deadlock
    }
  }

  // Stop and compute simulation digest
  size_t digest = world.Stop();

  // Print report
  std::cout << "Seed " << seed << " -> "
            << "digest: " << digest << ", time: " << world.TimeElapsed()
            << ", steps: " << world.StepCount() << std::endl;

  const auto event_log = world.EventLog();

  // Time limit exceeded
  if (world.GetCounter("requests") < kRequestsThreshold) {
    // Log
    std::cout << "Log:" << std::endl;
    matrix::WriteTextLog(event_log, std::cout);
    std::cout << std::endl;

    if (world.TimeElapsed() < kTimeLimit) {
      std::cout << "Deadlock in simulation" << std::endl;
    } else {
      std::cout << "Simulation time limit exceeded" << std::endl;
    }
    std::exit(1);
  }

  std::cout << "Requests completed: " << world.GetCounter("requests")
            << std::endl;

  // Check linearizability
  const auto history = world.History();
  const bool linearizable = histories::LinCheck<KVStoreModel>(history);

  if (!linearizable) {
    // Log
    std::cout << "Log:" << std::endl;
    matrix::WriteTextLog(event_log, std::cout);
    std::cout << std::endl;

    // History
    std::cout << "History (seed = " << seed
              << ") is NOT LINEARIZABLE:" << std::endl;
    histories::PrintKVHistory<Key, Value>(history, std::cout);

    FailTest();
  }

  return digest;
}

// Usage:
// 1) --det --sims 12345 - check determinism and run 12345 simulations
// 2) --seed 54321 - run single simulation with seed 54321

int main(int argc, const char** argv) {
  return matrix::Main(argc, argv, RunSimulation);
}
