#pragma once

#include <fstream>

namespace whirl::matrix::log {

void SetLogFile(const std::string& path);

std::ofstream GetLogFile();

}  // namespace whirl::matrix::log
