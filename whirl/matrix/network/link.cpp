#include <whirl/matrix/network/link.hpp>

#include <whirl/matrix/world/global.hpp>
#include <whirl/matrix/world/dice.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::net {

void Link::Add(Packet packet) {
  if (packet.type == EPacketType::Data) {
    Address to{EndHostName(), packet.dest_port};
    WHIRL_FMT_LOG("Send packet to {}: <{}>", to, packet.message);
  }
  packets_.Insert({packet, ChoosePacketDeliveryTime()});
}

TimePoint Link::ChoosePacketDeliveryTime() const {
  if (IsLoopBack()) {
    return GlobalNow() + 1;
  }
  return GlobalNow() + NetPacketDeliveryTime();
}

Packet Link::ExtractNextPacket() {
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

}  // namespace whirl::net
