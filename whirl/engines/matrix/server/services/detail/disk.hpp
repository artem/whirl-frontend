#pragma once

#include <whirl/services/time.hpp>

#include <whirl/engines/matrix/world/global/dice.hpp>

namespace whirl::matrix {

namespace detail {

class Disk {
 public:
  Disk(ITimeServicePtr time_service)
      : time_service_(std::move(time_service)) {
  }

  Future<void> Read() const {
    auto read_time = GetWorldBehaviour()->DiskRead();
    return time_service_->After(read_time);
  }

  Future<void> Write() {
    auto write_time = GetWorldBehaviour()->DiskWrite();
    return time_service_->After(write_time);
  }

 private:
  ITimeServicePtr time_service_;
};

}  // namespace detail

}  // namespace whirl::matrix
