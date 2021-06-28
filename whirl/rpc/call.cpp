#include <whirl/rpc/call.hpp>

#include <await/fibers/core/api.hpp>

namespace whirl::rpc {

namespace detail {

await::util::StopToken Caller1::DefaultStopToken() {
  if (await::fibers::AmIFiber()) {
    return await::fibers::self::LifetimeToken();
  }
  return await::util::NeverStop();
}

}  // namespace detail

}  // namespace whirl::rpc