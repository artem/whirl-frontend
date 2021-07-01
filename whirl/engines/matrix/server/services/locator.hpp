#pragma once

#include <whirl/node/runtime.hpp>
#include <whirl/engines/matrix/server/services/runtime.hpp>

namespace whirl::matrix {

class RuntimeLocator : public INodeRuntime {
 public:
  RuntimeLocator(NodeRuntime* runtime)
      : runtime_(runtime) {
  }

  const await::executors::IExecutorPtr& Executor() {
    return runtime_->thread_pool->GetExecutor();
  }

  ITimeServicePtr TimeService() {
    return runtime_->time.Get();
  }

  ITransportPtr NetTransport() {
    return runtime_->transport.Get();
  }

  IDatabasePtr Database() {
    return runtime_->db.Get();
  }

  IGuidGeneratorPtr GuidGenerator() {
    return runtime_->guids.Get();
  }

  IRandomServicePtr RandomService() {
    return runtime_->random.Get();
  }

  ITrueTimeServicePtr TrueTime() {
    return runtime_->true_time.Get();
  }

  IFileSystemPtr FileSystem() {
    return nullptr;  // TODO
  }

  IConfigPtr Config() {
    return runtime_->config.Get();
  }

  IDiscoveryPtr DiscoveryService() {
    return runtime_->discovery.Get();
  }

 private:
  NodeRuntime* runtime_;
};

}  // namespace whirl::matrix
