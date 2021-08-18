#include <whirl/program/main.hpp>

#include <whirl/logger/log.hpp>
#include <whirl/db/store/kv.hpp>

#include <whirl/fs/io.hpp>

#include <whirl/runtime/methods.hpp>

// Simulation
#include <whirl/engines/matrix/world/world.hpp>
#include <whirl/engines/matrix/test/event_log.hpp>

#include <wheels/support/view_util.hpp>
#include <wheels/io/read.hpp>

#include <chrono>

using namespace whirl;

//////////////////////////////////////////////////////////////////////

void TestNode() {
  node::main::Prologue();

  if (!node::rt::FileSystem()->Exists("/flag")) {
    node::fs::FileWriter file_writer(node::rt::FileSystem(), "/file");
    file_writer.Write(wheels::ViewOf("Hello, World!"));

    node::rt::Database()->Put("Test-Put", "Ok!");
    node::rt::Database()->Put("Test-Delete", "...");
    node::rt::Database()->Delete("Test-Delete");

    node::rt::FileSystem()->Create("/chunks/1");
    node::rt::FileSystem()->Create("/chunks/2");
    node::rt::FileSystem()->Create("/chunks/3");

    node::rt::FileSystem()->Create("/flag");
  }

  // Check database

  auto v1 = node::rt::Database()->TryGet("Test-Put");
  if (v1) {
    node::rt::PrintLine("Test-Put -> {}", *v1);
  }
  auto v2 = node::rt::Database()->TryGet("Test-Delete");
  if (!v2) {
    node::rt::PrintLine("Test-Delete key not found in database");
  }

  // Check file

  {
    node::fs::FileReader file_reader(node::rt::FileSystem(), "/file");
    auto content = wheels::io::ReadAll(&file_reader);
    node::rt::PrintLine("Content of '{}': <{}>", "/file", content);
  }

  // List directory

  {
    node::rt::PrintLine("List chunks:");
    auto chunks = node::rt::FileSystem()->ListFiles("/chunks/");
    for (const auto& fpath : chunks) {
      node::rt::PrintLine("\tChunk: {}", fpath);
    }
  }
}

//////////////////////////////////////////////////////////////////////

int main() {
  static const size_t kSeed = 17;

  matrix::World world{kSeed};

  std::string host = world.AddServer(TestNode);

  world.Start();
  world.MakeSteps(100);
  world.RestartServer(host);
  world.MakeSteps(100);

  auto std_out = world.GetStdout(host);

  size_t digest = world.Stop();

  std::cout
      << "Seed: " << kSeed
      << ", digest: " << digest
      << ", time: " << world.TimeElapsed()
      << ", steps: " << world.StepCount()
      << std::endl;

  std::cout << "Simulation log:" << std::endl;
  matrix::WriteTextLog(world.EventLog(), std::cout);

  std::cout << "Stdout of '" << host << "':" << std::endl;
  for (const auto& line : std_out) {
    std::cout << '\t' << line << std::endl;
  }

  std::cout << std::endl;

  return 0;
}