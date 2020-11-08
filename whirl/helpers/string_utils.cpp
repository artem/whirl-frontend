#include <whirl/helpers/string_utils.hpp>

namespace whirl {

Tokens Split(std::string str, char delimiter) {
  Tokens tokens;
  while (!str.empty()) {
    size_t pos = str.find(delimiter);
    if (pos == std::string::npos) {
      tokens.push_back(str);
      break;
    }
    tokens.push_back(str.substr(0, pos));
    str = str.substr(pos + 1);
  }
  return tokens;
}

}  // namespace whirl
