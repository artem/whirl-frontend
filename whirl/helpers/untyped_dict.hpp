#pragma once

#include <any>
#include <string>
#include <map>

namespace whirl {

class UntypedDict {
 public:
  void Set(const std::string& key, std::any value) {
    dict_.insert_or_assign(key, std::move(value));
  }

  std::any Get(const std::string& key) const {
    return dict_.at(key);
  }

 private:
  std::map<std::string, std::any> dict_;
};

}  // namespace whirl
