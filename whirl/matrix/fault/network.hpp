#pragma once

namespace whirl {

// Network

struct IFaultyNetwork {
  virtual ~IFaultyNetwork() = default;

  virtual void Split() = 0;

  // After Split
  virtual void Heal() = 0;
};

IFaultyNetwork& AccessFaultyNetwork();

}  // namespace whirl
