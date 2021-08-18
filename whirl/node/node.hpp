#pragma once

#include <memory>

namespace whirl::node {

//////////////////////////////////////////////////////////////////////

// Program running on physical server

struct INode {
  virtual ~INode() = default;
  virtual void Start() = 0;
};

using INodePtr = std::shared_ptr<INode>;

//////////////////////////////////////////////////////////////////////

struct INodeFactory {
  virtual ~INodeFactory() = default;

  virtual INodePtr CreateNode() = 0;
};

using INodeFactoryPtr = std::shared_ptr<INodeFactory>;

///////////////////////////////////////////////////////////////////

template <typename TNode>
class TNodeFactory : public INodeFactory {
  INodePtr CreateNode() override {
    return std::make_shared<TNode>();
  }
};

template <typename TNode>
INodeFactoryPtr Make() {
  return std::make_shared<TNodeFactory<TNode>>();
}

}  // namespace whirl::node
