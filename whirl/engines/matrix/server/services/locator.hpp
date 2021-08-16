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

  ITimeService* TimeService() {
    return runtime_->time.Get();
  }

  ITransport* NetTransport() {
    return runtime_->transport.Get();
  }

  node::db::IDatabase* Database() {
    return runtime_->db.Get();
  }

  IGuidGenerator* GuidGenerator() {
    return runtime_->guids.Get();
  }

  IRandomService* RandomService() {
    return runtime_->random.Get();
  }

  ITrueTimeService* TrueTime() {
    return runtime_->true_time.Get();
  }

  node::fs::IFileSystem* FileSystem() {
    return runtime_->fs.Get();
  }

  IConfig* Config() {
    return runtime_->config.Get();
  }

  IDiscoveryService* DiscoveryService() {
    return runtime_->discovery.Get();
  }

 private:
  NodeRuntime* runtime_;
};

}  // namespace whirl::matrix
