#pragma once

#include <whirl/rpc/impl/id.hpp>
#include <whirl/rpc/impl/raw.hpp>

#include <whirl/helpers/serialize.hpp>

#include <cereal/types/string.hpp>

#include <string>

namespace whirl::rpc {

// Network protocol

//////////////////////////////////////////////////////////////////////

// Request

struct RPCRequestMessage {
  RPCId id;
  std::string target;  // For debugging
  std::string method;
  RPCBytes input;

  SERIALIZE(CEREAL_NVP(id), CEREAL_NVP(target), CEREAL_NVP(method),
            CEREAL_NVP(input))
};

//////////////////////////////////////////////////////////////////////

// Response

struct RPCError {
  int code;
  std::string message;

  bool IsOk() const {
    return code == 0;
  }

  static RPCError Ok() {
    return {0, ""};
  }

  SERIALIZE(CEREAL_NVP(code), CEREAL_NVP(message))
};

struct RPCResponseMessage {
  RPCId request_id;
  std::string method;  // For debugging
  RPCBytes result;
  RPCError error;

  SERIALIZE(CEREAL_NVP(request_id), CEREAL_NVP(method), CEREAL_NVP(result),
            CEREAL_NVP(error))

  bool IsOk() const {
    return error.IsOk();
  }
};

}  // namespace whirl::rpc
