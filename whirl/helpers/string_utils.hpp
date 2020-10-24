#pragma once

#include <string>
#include <vector>

using Tokens = std::vector<std::string>;

Tokens Split(std::string str, char delimiter);
