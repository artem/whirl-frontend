#include <whirl/node/fs/path.hpp>
#include <whirl/node/fs/fs.hpp>

#include <whirl/runtime/access.hpp>

namespace whirl::node::fs {

Path::Path(std::string repr) : Path(GetRuntime().FileSystem(), repr) {
}

Path& Path::operator/=(const std::string& name) {
  repr_ = fs_->PathAppend(Repr(), name);
  return *this;
}

}  // namespace whirl::node::fs
