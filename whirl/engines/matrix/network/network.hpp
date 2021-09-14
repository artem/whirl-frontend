#pragma once

#include <whirl/engines/matrix/world/actor.hpp>
#include <whirl/engines/matrix/fault/network.hpp>

#include <whirl/engines/matrix/network/link.hpp>
#include <whirl/engines/matrix/network/server.hpp>

#include <whirl/engines/matrix/helpers/digest.hpp>

#include <timber/logger.hpp>

#include <string>
#include <cstdlib>
#include <vector>
#include <set>

namespace whirl::matrix::net {

// Link layer

using HostName = std::string;

class Network : public IActor, public fault::IFaultyNetwork {
  friend class Link;

  struct LinkEvent {
    TimePoint time;
    Link* link;

    bool operator<(const LinkEvent& that) const {
      return time < that.time;
    }
  };

  using LinkEvents = PriorityQueue<LinkEvent>;

 public:
  Network(timber::ILogBackend* log);

  // Non-copyable
  Network(const Network&) = delete;
  Network& operator=(const Network&) = delete;

  // Build network

  void AddServer(IServer* server);

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
  TimePoint NextStepTime() const override;
  void Step() override;

  void Shutdown() override;

  // IFaultyNetwork

  // - Links

  void PauseLink(const HostName& start, const HostName& end) override;

  void ResumeLink(const HostName& start, const HostName& end) override;

  // - Partitions

  void Split(const fault::Partition& lhs) override;
  void Heal() override;

  // Digest

  size_t Digest() const {
    return digest_.GetValue();
  }

 private:
  void AddLinkEvent(Link* link, TimePoint t);

  size_t ServerToIndex(const HostName& server) const;
  size_t GetLinkIndex(size_t i, size_t j) const;

  // After all `AddServer`
  void BuildLinks();

 private:
  std::vector<IServer*> servers_;
  std::vector<Link> links_;
  LinkEvents events_;

  DigestCalculator digest_;

  timber::Logger logger_;
};

}  // namespace whirl::matrix::net
