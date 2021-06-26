#include <whirl/engines/matrix/network/link.hpp>

#include <whirl/engines/matrix/network/network.hpp>

#include <whirl/engines/matrix/world/global/time.hpp>
#include <whirl/engines/matrix/world/global/behaviour.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::matrix::net {

void Link::Add(Packet packet) {
  if (packet.header.type == Packet::Type::Data) {
    Address to{End()->HostName(), packet.header.dest_port};
    WHIRL_LOG_INFO("Send packet to {}: <{}>", to, packet.message);
  }
  Add(std::move(packet), ChooseDeliveryTime(packet));
}

TimePoint Link::ChooseDeliveryTime(const Packet& packet) const {
  if (IsLoopBack()) {
    return GlobalNow() + 1;
  }
  return GlobalNow() + GetWorldBehaviour()->FlightTime(packet);
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

}  // namespace whirl::matrix::net
