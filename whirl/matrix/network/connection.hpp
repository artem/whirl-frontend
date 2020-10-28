#pragma once

#include <whirl/matrix/network/endpoint_id.hpp>
#include <whirl/matrix/network/link.hpp>

namespace whirl {

struct Connection {
  NetEndpointId client;
  NetEndpointId server;
  Link* link;
};

}  // namespace whirl
