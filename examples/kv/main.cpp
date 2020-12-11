#include <whirl/node/node_base.hpp>
#include <whirl/node/logging.hpp>
#include <whirl/rpc/use/service_base.hpp>
#include <whirl/helpers/serialize.hpp>

// Simulation
#include <whirl/matrix/world/world.hpp>
#include <whirl/matrix/client/client.hpp>
#include <whirl/matrix/history/printers/kv.hpp>
#include <whirl/matrix/history/checker/check.hpp>
#include <whirl/matrix/history/models/kv.hpp>
#include <whirl/matrix/world/global/random.hpp>
#include <whirl/matrix/world/global/vars.hpp>
#include <whirl/matrix/test/random.hpp>

#include <await/fibers/core/id.hpp>
#include <whirl/rpc/impl/id.hpp>

// Concurrency
#include <await/fibers/sync/future.hpp>
#include <await/fibers/core/await.hpp>
#include <await/futures/combine/quorum.hpp>

#include <cereal/types/string.hpp>
#include <fmt/ostream.h>

#include <random>

using namespace await::fibers;
using namespace whirl;

//////////////////////////////////////////////////////////////////////

// string -> int
using Key = std::string;
using Value = int;

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
  WHIRL_SERIALIZE(CEREAL_NVP(value), CEREAL_NVP(ts))
};

// For logging
std::ostream& operator<<(std::ostream& out, const StampedValue& v) {
  out << "{" << v.value << ", ts: " << v.ts << "}";
  return out;
}

//////////////////////////////////////////////////////////////////////

// KV storage node

class KVNode final : public rpc::ServiceBase<KVNode>,
                     public NodeBase,
                     public std::enable_shared_from_this<KVNode> {
 public:
  KVNode(NodeServices services)
      : NodeBase(std::move(services)), kv_(StorageBackend(), "kv") {
  }

 protected:
  // NodeBase
  void RegisterRPCServices(const rpc::IServerPtr& rpc_server) override {
    rpc_server->RegisterService("KV", shared_from_this());
  }

  // ServiceBase
  void RegisterRPCMethods() override {
    // Public
    RPC_REGISTER_METHOD(Set);
    RPC_REGISTER_METHOD(Get);

    // TODO: split coordinator / storage roles to different RPC services
    RPC_REGISTER_METHOD(Write);
    RPC_REGISTER_METHOD(Read);
  }

  // RPC method handlers

  // Public methods: Set and Get

  void Set(Key k, Value v) {
    Timestamp write_ts = ChooseWriteTimestamp();
    NODE_LOG_INFO("Write timestamp: {}", write_ts);

    std::vector<Future<void>> writes;
    for (size_t i = 0; i < PeerCount(); ++i) {
      writes.push_back(
          PeerChannel(i).Call("KV.Write", k, StampedValue{v, write_ts}));
    }

    // Await acks from majority of replicas
    Await(Quorum(std::move(writes), Majority())).ExpectOk();
  }

  Value Get(Key k) {
    std::vector<Future<StampedValue>> reads;

    // Broadcast KV.Read request
    for (size_t i = 0; i < PeerCount(); ++i) {
      reads.push_back(PeerChannel(i).Call("KV.Read", k));
    }

    // Await responses from majority of replicas
    auto values = Await(Quorum(std::move(reads), Majority())).Value();

    for (size_t i = 0; i < values.size(); ++i) {
      NODE_LOG_INFO("{}-th value in read quorum: {}", i + 1, values[i]);
    }

    auto winner = FindMostRecentValue(values);
    return winner.value;
  }

  // Internal storage methods

  void Write(Key k, StampedValue v) {
    std::optional<StampedValue> local = kv_.TryGet(k);

    if (!local.has_value()) {
      // First write for this key
      Update(k, v);
    } else {
      // Write timestamp > timestamp of locally stored value
      if (v.ts > local->ts) {
        Update(k, v);
      }
    }
  }

  void Update(Key k, StampedValue v) {
    NODE_LOG_INFO("Write '{}' -> {}", k, v);
    kv_.Set(k, v);
  }

  StampedValue Read(Key k) {
    return kv_.GetOr(k, StampedValue::NoValue());
  }

  Timestamp ChooseWriteTimestamp() {
    // Local wall clock may be out of sync with other replicas
    // Use TrueTime (TrueTime() method)
    return WallTimeNow();
  }

  // Find value with largest timestamp
  StampedValue FindMostRecentValue(
      const std::vector<StampedValue>& values) const {
    auto winner = values[0];
    for (size_t i = 1; i < values.size(); ++i) {
      if (values[i].ts > winner.ts) {
        winner = values[i];
      }
    }
    return winner;
  }

  // Quorum size
  size_t Majority() const {
    return PeerCount() / 2 + 1;
  }

 private:
  // Local persistent K/V storage
  // strings -> StampedValues
  LocalKVStorage<StampedValue> kv_;
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

const std::string& ChooseKey() {
  return kKeys.at(GlobalRandomNumber(GetGlobal<size_t>("num_keys")));
}

//////////////////////////////////////////////////////////////////////

class KVClient final : public ClientBase {
 public:
  KVClient(NodeServices services) : ClientBase(std::move(services)) {
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
        Value result = kv_store.Get(key);
        NODE_LOG_INFO("Get({}) -> {}", key, result);
      }

      GlobalCounter("requests").Increment();

      // Sleep for some time
      Threads().SleepFor(RandomNumber(1, 100));
    }
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

  // Log
  std::stringstream log;
  world.WriteLogTo(log);

  // Globals
  world.SetGlobal("num_keys", keys);
  world.InitCounter("requests", 0);

  // Run simulation
  world.Start();
  while (world.GetCounter("requests") < kRequestsThreshold &&
         world.TimeElapsed() < kTimeLimit) {
    world.Step();
  }

  // Stop and compute simulation digest
  size_t digest = world.Stop();

  // Print report
  std::cout << "Seed " << seed << " -> "
            << "digest: " << digest << ", time: " << world.TimeElapsed()
            << ", steps: " << world.StepCount() << std::endl;

  // Time limit exceeded
  if (world.GetCounter("requests") < kRequestsThreshold) {
    // Log
    std::cout << "Log:" << std::endl << log.str() << std::endl;
    std::cout << "Simulation time limit exceeded" << std::endl;
    std::exit(1);
  }

  std::cout << "Requests completed: "
            << world.GetCounter("requests") << std::endl;

  // Check linearizability
  const auto history = world.History();
  const bool linearizable = histories::LinCheck<KVStoreModel>(history);

  if (!linearizable) {
    // Log
    std::cout << "Log:" << std::endl << log.str() << std::endl;
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
