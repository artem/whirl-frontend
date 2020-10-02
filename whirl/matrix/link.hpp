#pragma once

namespace whirl {

class Link {
  bool IsLoopBack() const;
};

using Links = std::map<LinkEnds, Link>;

}  // namespace whirl
