#pragma once

#include <fstream>

namespace whirl {

void SetLogFile(const std::string& path);

std::ofstream GetLogFile();

}  // namespace whirl
