#pragma once

#include <whirl/services/local_storage_impl.hpp>

#include <whirl/helpers/serialize.hpp>

namespace whirl {

// Typed local storages

//////////////////////////////////////////////////////////////////////

// Persistent mapping: string -> V

template <typename V>
class LocalKVStorage {
 public:
  LocalKVStorage(ILocalStorageEnginePtr kv, const std::string& name)
      : impl_(kv), namespace_(MakeNamespace(name)) {
  }

  void Set(const Bytes& key, const V& value) {
    auto value_bytes = Serialize(value);
    impl_->Set(WithNamespace(key), value_bytes);
  }

  bool Has(const Bytes& key) {
    return impl_->Has(WithNamespace(key));
  }

  V Get(const Bytes& key) {
    auto value_bytes = impl_->Get(WithNamespace(key));
    return Deserialize<V>(value_bytes);
  }

 private:
  static std::string MakeNamespace(const std::string& name) {
    return std::string("kv:") + name + ":";
  }

  Bytes WithNamespace(const Bytes& user_key) const {
    return namespace_ + user_key;
  }

 private:
  ILocalStorageEnginePtr impl_;
  std::string namespace_;
};

//////////////////////////////////////////////////////////////////////

// Persistent map from string to arbitrary values

// Usage:
// * local_storage.Store("epoch", 42)
// * local_storage_.Load<int>("epoch");

class LocalStorage {
 public:
  LocalStorage(ILocalStorageEnginePtr impl) : impl_(impl) {
  }

  // TODO: remove
  LocalStorage() = default;

  bool Has(const Bytes& key) {
    return impl_->Has(WithNamespace(key));
  }

  template <typename U>
  void Store(const Bytes& key, const U& data) {
    auto data_bytes = Serialize(data);
    impl_->Set(WithNamespace(key), data_bytes);
  }

  template <typename U>
  U Load(const Bytes& key) {
    auto data_bytes = impl_->Get(WithNamespace(key));
    return Deserialize<U>(data_bytes);
  }

 private:
  Bytes WithNamespace(const Bytes& user_key) {
    static const std::string kNamespace = "local:";
    return kNamespace + user_key;
  }

 private:
  ILocalStorageEnginePtr impl_;
};

}  // namespace whirl
