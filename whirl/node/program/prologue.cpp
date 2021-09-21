#include <whirl/node/program/prologue.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

#include <await/fibers/core/await.hpp>

namespace whirl::node::program {

void Prologue() {
  await::fibers::self::SetName("main");

  rt::PrintLine("Starting at {}", rt::WallTimeNow());

  rt::Database()->Open(rt::Config()->GetString("db.path"));
}

}  // namespace whirl::node::program
