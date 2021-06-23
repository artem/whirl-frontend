#include <whirl/engines/matrix/log/file.hpp>

#include <whirl/engines/matrix/log/env.hpp>

#include <wheels/support/assert.hpp>
#include <wheels/support/env.hpp>

#include <iostream>

#if !defined(WHIRL_NO_FS)

#include <fmt/core.h>

#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

namespace whirl {

class LogFileManager {
 public:
  LogFileManager() {
  }

  void SetFile(const std::string& path) {
    Init(path);
  }

  std::ofstream NextLog() {
    if (!log_path_.has_value()) {
      Init(ChooseLogPath());
    }

    static const size_t kSimsPerLogFile = 10;

    if (++sims_ % kSimsPerLogFile == 0) {
      ResetLogFile();
    }

    // Open for append
    std::ofstream log{*log_path_, std::ofstream::out | std::ofstream::app};
    // Write simulation separator
    log << std::string(80, '-') << std::endl;
    return log;
  }

 private:
  void Init(fs::path path) {
    log_path_.emplace(path);

    if (!fs::exists(log_path_->parent_path())) {
      WHEELS_PANIC(
          "Log directory does not exist: " << log_path_->parent_path());
    }

    ResetLogFile();
    std::cout << "Simulator log file: " << *log_path_ << std::endl;
  }

  void ResetLogFile() {
    if (fs::exists(*log_path_)) {
      fs::resize_file(*log_path_, 0);
    }

    // Write header
    std::ofstream log(*log_path_);
    log << "Whirl simulator log" << std::endl;
    log.close();
  }

  static fs::path ChooseLogPath() {
    auto log_path = GetLogPathFromEnv();
    if (log_path.has_value()) {
      return *log_path;
    }
    return TempLogPath();
  }

  static fs::path TempLogPath() {
    auto tmp_path = fs::temp_directory_path();

    // Workaround for CLion remote build / manual build conflict
    auto user = wheels::GetUser();
    if (user.has_value()) {
      return tmp_path / fmt::format("whirl-{}.log", *user);
    } else {
      return tmp_path / "whirl.log";
    }
  }

 private:
  std::optional<fs::path> log_path_;
  size_t sims_{0};
};

static LogFileManager log_file_manager;

void SetLogFile(const std::string& path) {
  log_file_manager.SetFile(path);
}

std::ofstream GetLogFile() {
  return log_file_manager.NextLog();
}

}  // namespace whirl

#else

namespace whirl {

void SetLogFile(const std::string& path) {
  WHEELS_PANIC("Not supported");
}

std::ofstream GetLogFile() {
  auto log_path = GetLogPathFromEnv();
  WHEELS_VERIFY(log_path.has_value(),
                "Set simulator log path via WHIRL_LOG_FILE env var");
  return std::ofstream(*log_path, std::ofstream::out);
}

}  // namespace whirl

#endif
