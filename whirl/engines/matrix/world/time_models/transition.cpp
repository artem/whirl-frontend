#include <whirl/engines/matrix/world/time_models/transition.hpp>

#include <whirl/engines/matrix/world/global/time.hpp>

namespace whirl::matrix {

ITimeModelPtr MakeTransition(ITimeModelPtr /*before*/, ITimeModelPtr /*after*/,
                             TimePoint /*transition_time*/) {
  return nullptr;  // TODO
}

}  // namespace whirl::matrix
