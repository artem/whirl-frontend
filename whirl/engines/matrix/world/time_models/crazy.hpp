#pragma once

#include <whirl/engines/matrix/world/time_model.hpp>

namespace whirl::matrix {

// Asynchronous world
ITimeModelPtr MakeCrazyTimeModel();

}  // namespace whirl::matrix
