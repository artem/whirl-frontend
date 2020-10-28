#pragma once

#include <whirl/matrix/network/link.hpp>

#include <string>
#include <cstdlib>

#include <vector>
#include <deque>
#include <set>

namespace whirl {

// Link layer

using Partition = std::set<std::string>;

class LinkNetwork {
 public:
  LinkNetwork() = default;

  // Non-copyable
  LinkNetwork(const LinkNetwork& that) = delete;
  LinkNetwork& operator=(const LinkNetwork& that) = delete;

  // Build network

  void AddServer(const std::string& server);

  const auto& Servers() const {
    return servers_;
  }

  // After all `AddServer`
  void BuildLinks();

  // After `BuildLinks`
  Link* GetLink(const std::string& start, const std::string& end);

  // Acting

  bool IsRunnable() const;
  Link* NextPacketLink();
  void Shutdown();

  // Partitions

  void Split(const Partition& lhs);
  void Heal();

 private:
  size_t ServerToIndex(const std::string& server) const;
  size_t GetLinkIndex(size_t i, size_t j) const;

 private:
  std::vector<std::string> servers_;

  // NB: Persistent addresses
  std::deque<Link> links_;
};

}  // namespace whirl
