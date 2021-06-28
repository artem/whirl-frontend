#pragma once

#include <whirl/engines/matrix/world/time_model.hpp>

namespace whirl::matrix {

ITimeModelPtr MakeTransition(
    ITimeModelPtr before,
    ITimeModelPtr after,
    TimePoint transition_time);

}  // namespace whirl::matrix
