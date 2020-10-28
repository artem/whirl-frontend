#include <whirl/matrix/network/link.hpp>

#include <whirl/matrix/world/global.hpp>
#include <whirl/matrix/world/dice.hpp>

#include <wheels/support/assert.hpp>

namespace whirl {

void Link::Add(NetPacket packet) {
  WHIRL_FMT_LOG("Send packet from {} (endpoint {}) to {} (endpoint {}): <{}>",
                Start(), packet.source, End(), packet.dest, packet.message);
  packets_.Insert({packet, ChoosePacketDeliveryTime()});
}

TimePoint Link::ChoosePacketDeliveryTime() const {
  return GlobalNow() + NetPacketDeliveryTime();
}

NetPacket Link::ExtractNextPacket() {
  WHEELS_VERIFY(!paused_, "Link is paused");
  return packets_.Extract().packet;
}

void Link::Pause() {
  WHEELS_VERIFY(!paused_, "Link is already paused");
  paused_ = true;
}

void Link::Resume() {
  if (!paused_) {
    return;
  }

  paused_ = false;

  auto now = GlobalNow();

  if (!packets_.IsEmpty()) {
    while (packets_.Smallest().time < now) {
      auto packet = packets_.Extract();
      packet.time = now + 1;
      packets_.Insert(packet);
    }
  }
}

}  // namespace whirl
