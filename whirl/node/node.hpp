#pragma once

#include <whirl/node/config.hpp>
#include <whirl/node/services.hpp>

#include <vector>

namespace whirl {

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

  virtual INodePtr CreateNode(NodeServices services) = 0;
};

using INodeFactoryPtr = std::shared_ptr<INodeFactory>;

///////////////////////////////////////////////////////////////////

template <typename TNode>
class TNodeFactory : public INodeFactory {
  INodePtr CreateNode(NodeServices services) override {
    return std::make_shared<TNode>(std::move(services));
  }
};

template <typename TNode>
INodeFactoryPtr MakeNode() {
  return std::make_shared<TNodeFactory<TNode>>();
}

}  // namespace whirl
