#pragma once

namespace whirl::matrix {

// Network

struct IFaultyNetwork {
  virtual ~IFaultyNetwork() = default;

  virtual void Split() = 0;

  // After Split
  virtual void Heal() = 0;
};

}  // namespace whirl::matrix
