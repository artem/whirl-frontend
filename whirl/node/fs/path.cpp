#include <whirl/node/fs/path.hpp>
#include <whirl/node/fs/filesystem.hpp>

#include <whirl/runtime/access.hpp>

namespace whirl::node::fs {

Path::Path(std::string repr) : Path(GetRuntime().FileSystem(), repr) {
}

Path& Path::operator/=(const std::string& name) {
  repr_ = fs_->PathAppend(repr_, name);
  return *this;
}

}  // namespace whirl::node::fs
