#pragma once

#include <whirl/services/time.hpp>

#include <whirl/engines/matrix/world/global/time_model.hpp>

#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/future.hpp>

namespace whirl::matrix {

namespace detail {

class Disk {
 public:
  Disk(ITimeService* time_service) : time_service_(std::move(time_service)) {
  }

  void Read(size_t bytes) const {
    BlockFor(GetTimeModel()->DiskRead(bytes));
  }

  void Write(size_t bytes) {
    BlockFor(GetTimeModel()->DiskWrite(bytes));
  }

 private:
  void BlockFor(Duration latency) const {
    auto after = time_service_->After(latency);
    await::fibers::Await(std::move(after)).ExpectOk();
  }

 private:
  ITimeService* time_service_;
};

}  // namespace detail

}  // namespace whirl::matrix
