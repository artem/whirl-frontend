#include <whirl/matrix/log/file.hpp>

#include <whirl/matrix/log/env.hpp>

#include <wheels/support/panic.hpp>

#include <filesystem>
#include <cstdlib>
#include <iostream>

namespace fs = std::filesystem;

namespace whirl {

class LogFileManager {
 public:
  LogFileManager()
    : log_path_(LogPath()) {
    Init();

  }

  std::ofstream NextLog() {
    if (++sims_ % 10 == 0) {
      ResetLogFile();
    }
    std::ofstream log{log_path_, std::ofstream::out | std::ofstream::app};
    // Write simulation separator
    log << std::string(80, '-') << std::endl;
    return log;
  }

 private:
  void Init() {
    if (!fs::exists(log_path_.parent_path())) {
      WHEELS_PANIC("Log directory does not exist: " << log_path_.parent_path());
    }
    ResetLogFile();
    std::cout << "Simulator log file: " << log_path_ << std::endl;
  }

  void ResetLogFile() {
    if (fs::exists(log_path_)) {
      fs::resize_file(log_path_, 0);
    }

    // Write header
    std::ofstream fout(log_path_);
    fout << "Whirl simulator log" << std::endl;
    // TODO: date/time
    fout.close();
  }

  static fs::path LogPath() {
    auto log_path = GetLogPathFromEnv();
    if (log_path.has_value()) {
      return *log_path;
    }
    return TempLogPath();
  }

  static fs::path TempLogPath() {
    return fs::temp_directory_path() / "whirl.log";
  }

 private:
  fs::path log_path_;
  size_t sims_{0};
};

static LogFileManager log_file_manager;

std::ofstream GetLogFile() {
  return log_file_manager.NextLog();
}

}  // namespace whirl
