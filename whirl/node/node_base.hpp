#pragma once

#include <whirl/node/node.hpp>
#include <whirl/node/services.hpp>

#include <whirl/matrix/common/copy.hpp>

#include <wheels/support/assert.hpp>

#include <whirl/matrix/channels/logging.hpp>
#include <whirl/matrix/channels/retries.hpp>

namespace whirl {

using rpc::TRPCChannel;
using rpc::TRPCClient;
using rpc::TRPCServer;

class NodeBase : public INode {
 public:
  NodeBase(NodeServices services, NodeConfig config)
      : services_(std::move(services)), config_(std::move(config)) {
  }

  void SetCluster(std::vector<std::string> peers) override {
    peers_ = std::move(peers);
  }

  void Start() override {
    services_.threads.Spawn([this]() { Main(); });
  }

 private:
  void StartRPCServer() {
    services_.rpc_server.Start();
  }

  void ConnectToPeers() {
    for (const auto& peer : peers_) {
      channels_.push_back(WithRetries(
          MakeLoggingChannel(services_.rpc_client.MakeChannel(peer)),
          TimeService()));
    }
  }

 protected:
  // Me

  NodeId Id() const {
    return config_.id;
  }

  std::string MyName() const {
    return MakeCopy(config_.name);
  }

  // Cluster

  size_t PeerCount() const {
    return peers_.size();
  }

  const std::vector<std::string>& Peers() const {
    return peers_;
  }

  const std::string& PeerName(const size_t index) const {
    return peers_.at(index);
  }

  TRPCChannel& PeerChannel(size_t index) {
    return channels_.at(index);
  }

  // Shortcuts for common functions

  RandomUInt RandomNumber() {
    return services_.random->RandomNumber();
  }

  RandomUInt RandomNumber(size_t bound) {
    return RandomNumber() % bound;
  }

  RandomUInt RandomNumber(size_t lo, size_t hi) {
    return lo + RandomNumber(hi - lo);
  }

  TimePoint WallTimeNow() {
    return services_.time_service->WallTimeNow();
  }

  TimePoint MonotonicNow() {
    return services_.time_service->MonotonicNow();
  }

  Uid GenerateUid() {
    return services_.uids->Generate();
  }

  // Local services

  ThreadsRuntime& Threads() {
    return services_.threads;
  }

  LocalStorage& LocalStorage() {
    return services_.local_storage;
  }

  const ITimeServicePtr& TimeService() {
    return services_.time_service;
  }

  const ITrueTimeServicePtr& TrueTime() {
    return services_.true_time;
  }

  const ILocalStorageEnginePtr& StorageBackend() {
    return services_.storage_engine;
  }

  const INodeLoggerPtr& NodeLogger() {
    return services_.logger;
  }

 protected:
  // Override this methods

  virtual void RegisterRPCMethods(TRPCServer& /*rpc_server*/) {
  }

  virtual void MainThread() {
  }

 private:
  void Main() {
    await::fibers::SetName("main");

    StartRPCServer();
    RegisterRPCMethods(services_.rpc_server);
    ConnectToPeers();
    MainThread();
  }

 private:
  NodeServices services_;

  NodeConfig config_;

  std::vector<std::string> peers_;
  std::vector<TRPCChannel> channels_;
};

}  // namespace whirl
