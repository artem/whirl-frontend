#pragma once

#include <cstdlib>
#include <string>
#include <vector>

namespace whirl::matrix::fault {

void MakeStar(std::vector<std::string> pool, size_t center);

void MakeRandomStar(std::vector<std::string> pool);

}  // namespace whirl::matrix::fault
