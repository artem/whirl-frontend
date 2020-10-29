#pragma once

#include <whirl/rpc/impl/id.hpp>

#include <memory>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

struct Context {
  RPCId request_id;

  Context(RPCId id)
    : request_id(id) {
  }
};

using ContextPtr = std::shared_ptr<Context>;

//////////////////////////////////////////////////////////////////////

ContextPtr GetContext();

void SetContext(RPCId id);

}  // namespace whirl::rpc
