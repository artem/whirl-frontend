#pragma once

#include <whirl/node/node.hpp>
#include <whirl/node/services.hpp>

#include <whirl/rpc/use/channel.hpp>

namespace whirl {

class NodeBase : public INode {
 public:
  NodeBase(NodeServices services)
      : services_(std::move(services)) {
  }

  void Start() override;

 private:
  void StartRPCServer();
  void DiscoverCluster();
  rpc::IChannelPtr MakeChannel(const std::string& peer_addr);
  void ConnectToPeers();

 protected:
  const NodeServices& ThisNodeServices() {
    return services_;
  }

  // Me

  NodeId Id() const {
    return services_.config->Id();
  }

  // Cluster

  size_t PeerCount() const {
    return cluster_.size();
  }

  const std::vector<std::string>& Peers() const {
    return cluster_;
  }

  const std::string& PeerName(size_t index) const {
    return cluster_.at(index);
  }

  rpc::TChannel& PeerChannel(size_t index) {
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

  const ILocalStorageBackendPtr& StorageBackend() {
    return services_.storage_backend;
  }

  const ITrueTimeServicePtr& TrueTime() {
    return services_.true_time;
  }

  const INodeLoggerPtr& NodeLogger() {
    return services_.logger;
  }

 protected:
  // Override this methods

  virtual void RegisterRPCServices(const rpc::IServerPtr& /*rpc_server*/) {
  }

  virtual void MainThread() {
  }

 private:
  // Main fiber routine
  void Main();

 private:
  NodeServices services_;

  std::vector<std::string> cluster_;
  std::vector<rpc::TChannel> channels_;
};

}  // namespace whirl
