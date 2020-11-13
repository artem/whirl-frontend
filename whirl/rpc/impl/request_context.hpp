#pragma once

#include <whirl/rpc/impl/protocol.hpp>

#include <memory>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

class RequestContext {
 public:
  RequestContext(const RequestMessage& request) : request_(request) {
  }

  RequestId GetId() const {
    return request_.id;
  }

 private:
  RequestMessage request_;
};

using RequestContextPtr = std::shared_ptr<RequestContext>;

//////////////////////////////////////////////////////////////////////

RequestContextPtr GetRequestContext();

void SetRequestContext(const RequestMessage& request);

}  // namespace whirl::rpc
