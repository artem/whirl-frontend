#pragma once

#include <whirl/node/node.hpp>
#include <whirl/node/services.hpp>

#include <wheels/support/assert.hpp>

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
    Threads().Spawn([this]() { Main(); });
  }

 private:
  void StartRPCServer() {
    services_.rpc_server.Start();
  }

  rpc::IRPCChannelPtr MakeChannelTo(const std::string& peer);

  void ConnectToPeers() {
    for (const auto& peer : peers_) {
      channels_.push_back(MakeChannelTo(peer));
    }
  }

 protected:
  // Me

  NodeId Id() const {
    return config_.id;
  }

  const std::string& MyName() const {
    return config_.name;
  }

  // Cluster

  size_t PeerCount() const {
    return peers_.size();
  }

  const std::vector<std::string>& Peers() const {
    return peers_;
  }

  const std::string& PeerName(size_t index) const {
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

  const ITimeServicePtr& TimeService() {
    return services_.time_service;
  }

  LocalStorage& LocalStorage() {
    return services_.local_storage;
  }

  const ILocalStorageBackendPtr& StorageBackend() {
    return services_.storage_engine;
  }

  const ITrueTimeServicePtr& TrueTime() {
    return services_.true_time;
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
  // Main fiber routine
  void Main();

 private:
  NodeServices services_;

  NodeConfig config_;

  std::vector<std::string> peers_;
  std::vector<TRPCChannel> channels_;
};

}  // namespace whirl
