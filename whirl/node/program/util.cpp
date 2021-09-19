#include <whirl/node/program/util.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

#include <await/futures/util/never.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/future.hpp>

namespace whirl::node::main {

void Prologue() {
  await::fibers::self::SetName("main");

  rt::PrintLine("Starting at {}", rt::WallTimeNow());

  rt::Database()->Open(rt::Config()->GetString("db.path"));
}

}  // namespace whirl::node::main
