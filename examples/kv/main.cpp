#include <whirl/node/node_base.hpp>
#include <whirl/node/local_storage.hpp>
#include <whirl/node/logging.hpp>
#include <whirl/rpc/service_base.hpp>
#include <whirl/cereal/serializable.hpp>

// Simulation
#include <whirl/matrix/world/world.hpp>
#include <whirl/matrix/client/client.hpp>
#include <whirl/matrix/world/global/vars.hpp>
#include <whirl/matrix/test/random.hpp>

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

// KV storage node

class KVNode final : public rpc::ServiceBase<KVNode>,
                     public NodeBase,
                     public std::enable_shared_from_this<KVNode> {
 public:
  KVNode(NodeServices runtime)
      : NodeBase(std::move(runtime)), kv_(StorageBackend(), "kv") {
  }

 protected:
  // NodeBase
  void RegisterRPCServices(const rpc::IServerPtr& rpc_server) override {
    rpc_server->RegisterService("KV", shared_from_this());
  }

  // ServiceBase
  void RegisterRPCMethods() override {
    // TODO: split coordinator / storage roles to different RPC services

    // Coordinator RPC handlers
    RPC_REGISTER_METHOD(Set);
    RPC_REGISTER_METHOD(Get);

    // Storage replica RPC handlers
    RPC_REGISTER_METHOD(LocalWrite);
    RPC_REGISTER_METHOD(LocalRead);
  }

  // RPC method handlers

  // Coordinator role

  void Set(Key key, Value value) {
    Timestamp write_ts = ChooseWriteTimestamp();
    NODE_LOG_INFO("Write timestamp: {}", write_ts);

    std::vector<Future<void>> writes;
    // TODO: iterate over configuration
    for (size_t i = 0; i < PeerCount(); ++i) {
      writes.push_back(
          // TODO: replace by rpc::Call
          PeerChannel(i).Call("KV.LocalWrite", key,
                              StampedValue{value, write_ts}));
    }

    // Await acks from majority of replicas
    // Await(std::move(future)).ExpectOk() ~ future.await? in Rust
    Await(Quorum(std::move(writes), Majority())).ExpectOk();
  }

  Value Get(Key key) {
    std::vector<Future<StampedValue>> reads;

    // Broadcast KV.LocalRead request
    for (size_t i = 0; i < PeerCount(); ++i) {
      reads.push_back(PeerChannel(i).Call("KV.LocalRead", key));
    }

    // Await responses from majority of replicas

    // Steps:
    // 1) Combine futures from read RPC-s to single quorum future
    Future<std::vector<StampedValue>> quorum_reads =
        Quorum(std::move(reads), Majority());
    // 2) Block current fiber until quorum collected
    Result<std::vector<StampedValue>> results = Await(std::move(quorum_reads));
    // 3) Unpack vector or throw error
    auto values = results.Value();

    // Or just combine all steps to:
    // auto values = Await(Quorum(std::move(reads), Majority())).Value()

    for (size_t i = 0; i < values.size(); ++i) {
      NODE_LOG_INFO("{}-th value in read quorum: {}", i + 1, values[i]);
    }

    auto most_recent = FindMostRecentValue(values);
    return most_recent.value;
  }

  // Storage replica role

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
    NODE_LOG_INFO("Write '{}' -> {}", key, stamped_value);
    kv_.Set(key, stamped_value);
  }

  StampedValue LocalRead(Key key) {
    std::lock_guard guard(mutex_);  // Blocks fiber, not thread!
    return kv_.GetOr(key, StampedValue::NoValue());
  }

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
  // Local persistent K/V storage
  // strings -> StampedValues
  LocalKVStorage<StampedValue> kv_;
  // Fiber-aware mutex
  // Guards access to kv_ from RPC handlers
  await::fibers::Mutex mutex_;
};

//////////////////////////////////////////////////////////////////////

class KVBlockingStub {
 public:
  KVBlockingStub(rpc::TChannel& channel) : channel_(channel) {
  }

  void Set(Key k, Value v) {
    Await(channel_.Call("KV.Set", k, v).As<void>()).ExpectOk();
  }

  Value Get(Key k) {
    return Await(channel_.Call("KV.Get", k).As<Value>()).Value();
  }

 private:
  rpc::TChannel& channel_;
};

//////////////////////////////////////////////////////////////////////

static const std::vector<std::string> kKeys({"a", "b", "c"});

//////////////////////////////////////////////////////////////////////

class KVClient final : public ClientBase {
 public:
  KVClient(NodeServices runtime) : ClientBase(std::move(runtime)) {
  }

 protected:
  void MainThread() override {
    KVBlockingStub kv_store{Channel()};

    for (size_t i = 1;; ++i) {
      if (RandomNumber() % 2 == 0) {
        Key key = ChooseKey();
        Value value = RandomNumber(1, 100);
        NODE_LOG_INFO("Execute Set({}, {})", key, value);
        kv_store.Set(key, value);
        NODE_LOG_INFO("Set completed");
      } else {
        Key key = ChooseKey();
        NODE_LOG_INFO("Execute Get({})", key);
        [[maybe_unused]] Value result = kv_store.Get(key);
        NODE_LOG_INFO("Get({}) -> {}", key, result);
      }

      GlobalCounter("requests").Increment();

      // Random pause
      SleepFor(RandomNumber(1, 100));
    }
  }

 private:
  const std::string& ChooseKey() const {
    return kKeys.at(RandomNumber(GetGlobal<size_t>("keys")));
  }
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

  Random random{seed};

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

  World world{seed};

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

  const auto text_log = world.TextLog();

  // Time limit exceeded
  if (world.GetCounter("requests") < kRequestsThreshold) {
    // Log
    std::cout << "Log:" << std::endl << text_log << std::endl;
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
    std::cout << "Log:" << std::endl << text_log << std::endl;
    // History
    std::cout << "History (seed = " << seed
              << ") is NOT LINEARIZABLE:" << std::endl;
    histories::PrintKVHistory<Key, Value>(history, std::cout);

    FailTest();
  }

  return digest;
}

void TestDeterminism() {
  static const size_t kSeed = 104107713;

  std::cout << "Test determinism:" << std::endl;

  size_t digest = RunSimulation(kSeed);

  // Repeat with the same seed
  if (RunSimulation(kSeed) != digest) {
    std::cout << "Impl is not deterministic" << std::endl;
    FailTest();
  }
}

void RunSimulations(size_t count) {
  std::mt19937 seeds{42};

  std::cout << "Run simulations:" << std::endl;

  for (size_t i = 1; i <= count; ++i) {
    std::cout << "Simulation " << i << "..." << std::endl;
    RunSimulation(seeds());
  }
}

int main() {
  TestDeterminism();
  RunSimulations(12345);

  std::cout << std::endl << "Looks good! ヽ(‘ー`)ノ" << std::endl;

  return 0;
}
