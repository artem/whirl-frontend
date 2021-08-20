#pragma once

#include <string>
#include <set>
#include <vector>

namespace whirl::matrix::fault {

// Network

//////////////////////////////////////////////////////////////////////

using Partition = std::set<std::string>;

//////////////////////////////////////////////////////////////////////

struct IFaultyNetwork {
  virtual ~IFaultyNetwork() = default;

  // Links

  virtual void PauseLink(
      const std::string& start, const std::string& end) = 0;

  virtual void ResumeLink(
      const std::string& start, const std::string& end) = 0;

  // Partitions

  virtual void Split(const Partition& lhs) = 0;

  // After Split
  virtual void Heal() = 0;
};

}  // namespace whirl::matrix
