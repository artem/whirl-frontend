#pragma once

#include <whirl/matrix/world/actor.hpp>
#include <whirl/matrix/world/faults.hpp>

#include <whirl/matrix/network/link.hpp>
#include <whirl/matrix/network/server.hpp>

#include <whirl/matrix/log/logger.hpp>

#include <string>
#include <cstdlib>
#include <vector>
#include <set>

namespace whirl::net {

// Link layer

using HostName = std::string;

using Partition = std::set<HostName>;

class Network : public IActor, public IFaultyNetwork {
 public:
  Network() = default;

  // Non-copyable
  Network(const Network& that) = delete;
  Network& operator=(const Network& that) = delete;

  // Build network

  void AddServer(INetServer* server);

  // After `BuildLinks`
  Link* GetLink(const HostName& start, const HostName& end);

  // IActor

  const std::string& Name() const override {
    static const std::string kName = "Network";
    return kName;
  }

  // After `AddServer`
  void Start() override;

  bool IsRunnable() const override;
  TimePoint NextStepTime() override;
  void Step() override;

  void Shutdown() override;

  // Partitions

  void Split() override;
  void Split(const Partition& lhs);
  void Heal() override;

 private:
  size_t ServerToIndex(const HostName& server) const;
  size_t GetLinkIndex(size_t i, size_t j) const;

  // After all `AddServer`
  void BuildLinks();

  Link* FindLinkWithNextPacket();

 private:
  std::vector<INetServer*> servers_;
  std::vector<Link> links_;

  Logger logger_{"Network"};
};

}  // namespace whirl::net
