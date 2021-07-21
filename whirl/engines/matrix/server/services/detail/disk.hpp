#pragma once

#include <whirl/services/time.hpp>

#include <whirl/engines/matrix/world/global/time_model.hpp>

#include <await/futures/core/future.hpp>

namespace whirl::matrix {

namespace detail {

class Disk {
 public:
  Disk(ITimeServicePtr time_service) : time_service_(std::move(time_service)) {
  }

  await::futures::Future<void> Read() const {
    auto read_time = GetTimeModel()->DiskRead();
    return time_service_->After(read_time);
  }

  await::futures::Future<void> Write() {
    auto write_time = GetTimeModel()->DiskWrite();
    return time_service_->After(write_time);
  }

 private:
  ITimeServicePtr time_service_;
};

}  // namespace detail

}  // namespace whirl::matrix
