#include <whirl/node/program/main.hpp>

#include <whirl/node/runtime/methods.hpp>

#include <await/futures/util/never.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/future.hpp>

namespace whirl::node::main {

void Prologue() {
  await::fibers::self::SetName("main");

  rt::PrintLine("Starting at T{}", rt::WallTimeNow());

  // TODO: Read dir from config
  rt::Database()->Open("/db");
}

void BlockForever() {
  await::fibers::Await(await::futures::Never()).ExpectOk();
  std::abort();
}

}  // namespace whirl::node::main
