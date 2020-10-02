#pragma once

#include <whirl/services/node_services.hpp>

#include <vector>

namespace whirl {

//////////////////////////////////////////////////////////////////////

using NodeId = size_t;

//////////////////////////////////////////////////////////////////////

struct NodeConfig {
  NodeId id;
  std::string name;

  NodeConfig(NodeId _id) : id(_id) {
  }

  NodeConfig(NodeId _id, const std::string& _name) : id(_id), name(_name) {
  }
};

//////////////////////////////////////////////////////////////////////

using Peers = std::vector<std::string>;

//////////////////////////////////////////////////////////////////////

struct INode {
  virtual ~INode() = default;
  virtual void Start() = 0;
  virtual void JoinCluster(Peers peers) = 0;
};

using INodePtr = std::unique_ptr<INode>;

//////////////////////////////////////////////////////////////////////

struct INodeFactory {
  virtual ~INodeFactory() = default;

  virtual INodePtr CreateNode(NodeServices services, NodeConfig config) = 0;
};

using INodeFactoryPtr = std::shared_ptr<INodeFactory>;

///////////////////////////////////////////////////////////////////

template <typename TNode>
class TNodeFactory : public INodeFactory {
  INodePtr CreateNode(NodeServices services, NodeConfig config) override {
    return std::make_unique<TNode>(std::move(services), std::move(config));
  }
};

template <typename TNode>
INodeFactoryPtr NodeFactory() {
  return std::make_shared<TNodeFactory<TNode>>();
}

}  // namespace whirl
