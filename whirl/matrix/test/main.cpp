#include <whirl/matrix/test/main.hpp>

#include <wheels/support/argparse.hpp>

#include <iostream>
#include <sstream>
#include <random>

namespace whirl {

void FailTest() {
  std::cout << "(ﾉಥ益ಥ）ﾉ ┻━┻" << std::endl;
  std::cout.flush();
  std::exit(1);
}

void TestDeterminism(Simulation sim) {
  static const size_t kSeed = 104107713;

  std::cout << "Test determinism with seed " << kSeed << ":" << std::endl;

  size_t digest1 = sim(kSeed);
  size_t digest2 = sim(kSeed);

  if (digest1 != digest2) {
    std::cout << "Impl is not deterministic: digest1 = " << digest1 << ", digest2 = " << digest2 << std::endl;
    FailTest();
  }
}

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

static void CLI(wheels::ArgumentParser& parser) {
  parser.AddHelpFlag();

  parser.Add("det")
      .Flag()
      .Help("Test determinism");

  parser.Add("sims")
      .ValueDescr("uint")
      .Help("Number of simulations to run");

  parser.Add("seed")
      .ValueDescr("uint")
      .WithDefault("-");  // TODO
}

template <typename T>
static T FromString(std::string str) {
  std::istringstream input{str};
  T value;
  input >> value;
  return value;
}

int MatrixMain(int argc, const char** argv, Simulation sim) {
  wheels::ArgumentParser parser{"Whirl simulator"};
  CLI(parser);

  wheels::ParsedArgs args = parser.Parse(argc, argv);

  auto seed_opt = args.Get("seed");
  if (seed_opt != "-") {
    RunSimulation(sim, FromString<size_t>(seed_opt));
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

}  // namespace whirl
