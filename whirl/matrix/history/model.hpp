#pragma once

#include <whirl/matrix/history/history.hpp>

namespace whirl::history {

//////////////////////////////////////////////////////////////////////

struct IModel {
  virtual ~IModel() = default;

  virtual Value Apply(
      const std::string& method,
      Arguments& arguments) = 0;
};

}  // namespace whirl::history
