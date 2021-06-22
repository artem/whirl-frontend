#include <whirl/matrix/network/link.hpp>

#include <whirl/matrix/network/network.hpp>

#include <whirl/matrix/world/global/time.hpp>
#include <whirl/matrix/world/dice.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::net {

void Link::Add(Packet packet) {
  if (packet.header.type == EPacketType::Data) {
    Address to{EndHostName(), packet.header.dest_port};
    WHIRL_SIM_LOG("Send packet to {}: <{}>", to, packet.message);
  }
  Add(std::move(packet), ChooseDeliveryTime(packet));
}

TimePoint Link::ChooseDeliveryTime(const Packet& packet) const {
  if (IsLoopBack()) {
    return GlobalNow() + 1;
  }
  return GlobalNow() + FlightTime(packet);
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
      Add(packets_.Extract().packet, now + 1);
    }
  }
}

void Link::Add(Packet&& packet, TimePoint delivery_time) {
  packets_.Insert({packet, delivery_time});
  net_->AddLinkEvent(this, delivery_time);
}

}  // namespace whirl::net
