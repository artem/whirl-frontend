#pragma once

#include <cstdlib>
#include <memory>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Servers

struct IFaultyServer {
  virtual ~IFaultyServer() = default;

  virtual const std::string& Name() const = 0;

  virtual void Pause() = 0;
  virtual void Resume() = 0;

  virtual void Reboot() = 0;

  virtual void AdjustWallClock() = 0;
};

IFaultyServer& AccessFaultyServer(size_t index);

//////////////////////////////////////////////////////////////////////

// Network

struct IFaultyNetwork {
  virtual ~IFaultyNetwork() = default;

  virtual void Split() = 0;
  virtual void Heal() = 0;
};

IFaultyNetwork& AccessFaultyNetwork();

}  // namespace whirl
