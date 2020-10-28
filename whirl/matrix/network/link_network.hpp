#pragma once

#include <whirl/matrix/network/link.hpp>

#include <string>
#include <cstdlib>

#include <vector>
#include <deque>

namespace whirl {

// Link layer

class LinkNetwork {
 public:
  LinkNetwork() = default;

  // Non-copyable
  LinkNetwork(const LinkNetwork& that) = delete;
  LinkNetwork& operator=(const LinkNetwork& that) = delete;

  // Build network

  void AddServer(const std::string& server);

  // After all `AddServer`
  void BuildLinks();

  // After `BuildLinks`
  Link* GetLink(const std::string& start, const std::string& end);

  // Acting

  bool IsRunnable() const;
  Link* NextPacketLink();
  void Shutdown();

 private:
  size_t ServerToIndex(const std::string& server) const;
  size_t GetLinkIndex(size_t i, size_t j) const;

 private:
  std::vector<std::string> servers_;

  // NB: Persistent addresses
  std::deque<Link> links_;
};

}  // namespace whirl
