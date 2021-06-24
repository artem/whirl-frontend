#pragma once

#include <fstream>

namespace whirl::matrix {

void SetLogFile(const std::string& path);

std::ofstream GetLogFile();

}  // namespace whirl::matrix
