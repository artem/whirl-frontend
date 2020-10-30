#pragma once

#include <cstdlib>
#include <memory>
#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Servers

struct IFaultyServer {
  virtual ~IFaultyServer() = default;

  virtual const std::string& Name() const = 0;

  virtual bool IsAlive() const = 0;

  virtual void Pause() = 0;
  // After Pause
  virtual void Resume() = 0;

  virtual void Crash() = 0;
  // After Crash
  virtual void Start() = 0;

  virtual void FastReboot() = 0;

  virtual void AdjustWallClock() = 0;
};

IFaultyServer& AccessFaultyServer(size_t index);

//////////////////////////////////////////////////////////////////////

// Network

struct IFaultyNetwork {
  virtual ~IFaultyNetwork() = default;

  virtual void Split() = 0;

  // After Split
  virtual void Heal() = 0;
};

IFaultyNetwork& AccessFaultyNetwork();

}  // namespace whirl
