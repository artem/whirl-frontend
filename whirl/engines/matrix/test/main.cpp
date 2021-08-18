#include <whirl/engines/matrix/test/main.hpp>

#include <whirl/engines/matrix/log/file.hpp>
#include <whirl/engines/matrix/memory/new.hpp>

#include <wheels/cmdline/argparse.hpp>

#include <iostream>
#include <sstream>
#include <random>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

void FailTest() {
  std::cout << "(ﾉಥ益ಥ）ﾉ ┻━┻" << std::endl;
  std::cout.flush();
  std::exit(1);
}

//////////////////////////////////////////////////////////////////////

void TestDeterminism(Simulation sim) {
  static const size_t kSeed = 104107713;

  std::cout << "Test determinism with seed " << kSeed << ":" << std::endl;

  size_t digest1 = sim(kSeed);

  auto allocs_checksum_1 = GlobalAllocsCheckSum();

  size_t digest2 = sim(kSeed);

  auto allocs_checksum_2 = GlobalAllocsCheckSum();

  if (digest1 != digest2) {
    std::cerr << "Simulation is not deterministic: digest1 = " << digest1
              << ", digest2 = " << digest2 << std::endl;
    FailTest();
  }

  // Simulator internal check
  if (allocs_checksum_1 != allocs_checksum_2) {
    std::cerr << "Global allocator checksums mismatch" << std::endl;
    FailTest();
  }
}

//////////////////////////////////////////////////////////////////////

void RunSimulations(Simulation sim, size_t count) {
  std::mt19937 seeds{42};

  std::cout << "Run simulations:" << std::endl;

  for (size_t i = 1; i <= count; ++i) {
    std::cout << "Simulation " << i << "..." << std::endl;
    sim(seeds());
  }
}

void RunSimulation(Simulation sim, size_t seed) {
  std::cout << "Run single simulation with seed = " << seed << std::endl;

  size_t digest = sim(seed);

  std::cout << "Simulation digest = " << digest << std::endl;
}

//////////////////////////////////////////////////////////////////////

static void CLI(wheels::ArgumentParser& parser) {
  parser.AddHelpFlag();

  parser.Add("det").Flag().Help("Test determinism");
  parser.Add("sims").ValueDescr("uint").Help("Number of simulations to run");
  parser.Add("seed").ValueDescr("uint").Optional();
  parser.Add("log").ValueDescr("path").Optional();
}

template <typename T>
static T FromString(std::string str) {
  std::istringstream input{str};
  T value;
  input >> value;
  return value;
}

//////////////////////////////////////////////////////////////////////

int Main(int argc, const char** argv, Simulation sim) {
  // For simulator debugging
  // ActivateAllocsTracker();

  wheels::ArgumentParser parser{"Whirl simulator"};
  CLI(parser);

  wheels::ParsedArgs args = parser.Parse(argc, argv);

  if (args.Has("log")) {
    std::string log_fpath = args.Get("log");
    SetLogFile(log_fpath);
  }
  // Initialize
  GetLogFile();

  if (args.Has("seed")) {
    RunSimulation(sim, FromString<size_t>(args.Get("seed")));
    return 0;
  }

  if (args.HasFlag("det")) {
    TestDeterminism(sim);
  }

  size_t sims_count = FromString<size_t>(args.Get("sims"));
  RunSimulations(sim, sims_count);

  std::cout << std::endl << "Looks good! ヽ(‘ー`)ノ" << std::endl;

  return 0;
}

}  // namespace whirl::matrix
