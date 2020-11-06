#pragma once

#include <whirl/matrix/world/world.hpp>

#include <iostream>
#include <string>

namespace whirl {

namespace detail {

struct Sink {
  template <typename... Args>
  Sink(Args const&...) {
  }
};

}  // namespace detail

class TestReporter {
 public:
  template <typename... Args>
  void PrintLine(const char* format, Args&&... args) {
    fmt::print(format, std::forward<Args>(args)...);
    std::cout << std::endl;
  }

  template <typename... Args>
  void DebugLine(const char* format, Args&&... args) {
#ifndef NDEBUG
    fmt::print(format, std::forward<Args>(args)...);
    std::cout << std::endl;
#else
    detail::Sink{format, args...};
#endif
  }

  void PrintHeaderLine() {
    static const std::string kSeparatorLine(80, '-');
  }

  void PrintSimSeed(size_t seed) {
    DebugLine("Simulation seed: {}", seed);
  }

  void PrintSimReport(const World& world) {
#ifndef NDEBUG
    std::cout << "Seed " << world.Seed() << " -> "
              << "digest: " << fmt::format("{:x}", world.Digest())
              << ", time: " << world.TimeElapsed()
              << ", steps: " << world.StepCount() << std::endl;
#else
    (void)world;
#endif
  }

  void PrintSimLog(const std::string& log) {
#ifndef NDEBUG
    std::cout << "Log: " << log << std::endl;
#else
    (void)log;
#endif
  }

  template <typename Model>
  void PrintSimHistory(const histories::History& history) {
    using Printer = typename Model::CallPrinter;
    histories::Print<Printer>(history, std::cout);
  }

  void ResetSimCount() {
    sims_ = 0;
  }

  void StartSim() {
    ++sims_;
#ifndef NDEBUG
    static const std::string kSeparatorLine(80, '-');

    std::cout << kSeparatorLine << std::endl;
    std::cout << "Simulation " << sims_ << ": " << std::endl;
#endif
  }

  size_t SimIndex() const {
    return sims_;
  }

  void CompleteSim() {
#ifdef NDEBUG
    if (sims_ > 0 && sims_ % 1000 == 0) {
      std::cout << "Simulations completed: " << sims_ << std::endl;
    }
#endif
  }

  void Congratulate() {
    std::cout << std::endl << "Looks good! ヽ(‘ー`)ノ" << std::endl;
  }

  void Fail() {
    std::cout << "(ﾉಥ益ಥ）ﾉ ┻━┻" << std::endl;
    std::exit(1);
  }

  void Fail(const std::string error) {
    std::cout << error << std::endl;
    Fail();
  }

 private:
  size_t sims_ = 0;
};

}  // namespace whirl
