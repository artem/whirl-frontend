#pragma once

namespace whirl {

struct IFaultInjector {
  virtual ~IFaultInjector() = default;

  virtual void ThreadPause() = 0;
  virtual void Reboot() = 0;
  virtual void AdjustWallClock() = 0;
};

}  // namespace whirl
