#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <functional>

#include <fmt/ostream.h>

namespace whirl::node::fs {

//////////////////////////////////////////////////////////////////////

struct IFileSystem;

class Path {
 public:
  Path(const IFileSystem* fs, std::string repr)
      : fs_(fs), repr_(std::move(repr)) {
  }

  // Access fs via node runtime
  Path(std::string str);

  Path(const char* str) : Path(std::string{str}) {
  }

  const std::string& Repr() const {
    return repr_;
  }

  bool operator==(const Path& that) const {
    return repr_ == that.repr_;
  }

  bool operator!=(const Path& that) const {
    return !(*this == that);
  }

  // For ordered containers
  bool operator<(const Path& path) const {
    return repr_ < path.repr_;
  }

  std::size_t ComputeHash() const noexcept {
    return std::hash<std::string>()(repr_);
  }

  Path& operator/=(const std::string& name);

 private:
  const IFileSystem* fs_;
  std::string repr_;
};

inline std::ostream& operator<<(std::ostream& out, const Path& path) {
  out << path.Repr();
  return out;
}

using FileList = std::vector<Path>;

}  // namespace whirl::node::fs

//////////////////////////////////////////////////////////////////////

namespace std {

template <>
struct hash<whirl::node::fs::Path> {
  std::size_t operator()(const whirl::node::fs::Path& path) const noexcept {
    return path.ComputeHash();
  }
};

}  // namespace std
