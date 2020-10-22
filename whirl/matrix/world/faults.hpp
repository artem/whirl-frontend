#pragma once

#include <cstdlib>
#include <memory>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Servers

struct IFaultyServer {
  virtual ~IFaultyServer() = default;

  virtual void Pause() = 0;
  virtual void Resume() = 0;

  virtual void Reboot() = 0;

  virtual void AdjustWallTime() = 0;
};

IFaultyServer& AccessFaultyServer(size_t index);

//////////////////////////////////////////////////////////////////////

// Network

struct IFaultyNetwork {
  virtual ~IFaultyNetwork() = default;

  // TODO
};

IFaultyNetwork& AccessFaultyNetwork();

}  // namespace whirl
