#pragma once

#include <whirl/matrix/network/link.hpp>

#include <string>
#include <cstdlib>
#include <vector>
#include <set>

namespace whirl {

// Link layer

using ServerName = std::string;

using Partition = std::set<ServerName>;

class LinkLayer {
 public:
  LinkLayer() = default;

  // Non-copyable
  LinkLayer(const LinkLayer& that) = delete;
  LinkLayer& operator=(const LinkLayer& that) = delete;

  // Build network

  void AddServer(const ServerName& server);

  const auto& Servers() const {
    return servers_;
  }

  // After all `AddServer`
  void BuildLinks();

  // After `BuildLinks`
  Link* GetLink(const ServerName& start, const ServerName& end);

  // Acting

  bool IsRunnable() const;
  Link* FindLinkWithNextPacket();
  void Shutdown();

  // Partitions

  void Split(const Partition& lhs);
  void Heal();

 private:
  size_t ServerToIndex(const ServerName& server) const;
  size_t GetLinkIndex(size_t i, size_t j) const;

 private:
  std::vector<ServerName> servers_;
  std::vector<Link> links_;
};

}  // namespace whirl
