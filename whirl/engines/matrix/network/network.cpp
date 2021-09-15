#include <whirl/engines/matrix/network/network.hpp>

#include <whirl/engines/matrix/memory/new.hpp>
#include <whirl/engines/matrix/world/global/random.hpp>

#include <timber/log.hpp>

#include <wheels/support/assert.hpp>
#include <wheels/support/compiler.hpp>

namespace whirl::matrix::net {

Network::Network(timber::ILogBackend* log) : logger_("Network", log) {
}

void Network::AddServer(IServer* server) {
  servers_.push_back(server);
}

void Network::BuildLinks() {
  // Create one-way link between each pair of servers
  for (size_t i = 0; i < servers_.size(); ++i) {
    for (size_t j = 0; j < servers_.size(); ++j) {
      links_.emplace_back(this, servers_[i], servers_[j]);
    }
  }

  // Link opposite physical links
  for (size_t i = 0; i < servers_.size(); ++i) {
    for (size_t j = i; j < servers_.size(); ++j) {
      size_t ij = GetLinkIndex(i, j);
      size_t ji = GetLinkIndex(j, i);

      links_[ij].SetOpposite(&links_[ji]);
      links_[ji].SetOpposite(&links_[ij]);
    }
  }
}

Link* Network::GetLink(const HostName& start, const HostName& end) {
  size_t i = ServerToIndex(start);
  size_t j = ServerToIndex(end);

  return &links_.at(GetLinkIndex(i, j));
}

// IActor

void Network::Start() {
  BuildLinks();
}

bool Network::IsRunnable() const {
  return !events_.IsEmpty();
}

TimePoint Network::NextStepTime() const {
  return events_.Smallest().time;
}

void Network::Step() {
  LinkEvent event = events_.Extract();
  Link* link = event.link;

  if (link->IsPaused()) {
    return;  // Skip this step
  }

  WHEELS_VERIFY(link->HasPackets(), "Broken net");
  WHEELS_VERIFY(link->NextPacketTime() == event.time, "Broken net");

  Packet packet = link->ExtractNextPacket();

  // ???
  // digest_.EatT(packet.message);
  digest_.Eat(packet.header.source_port)
      .Eat(packet.header.dest_port)
      .Eat(packet.message.length());

  IServer* receiver = link->End();
  receiver->HandlePacket(packet, link->GetOpposite());
}

void Network::AddLinkEvent(Link* link, TimePoint t) {
  events_.Insert({t, link});
}

void Network::Shutdown() {
  events_.Clear();
  for (auto& link : links_) {
    link.Shutdown();
  }
}

size_t Network::ServerToIndex(const HostName& hostname) const {
  for (size_t i = 0; i < servers_.size(); ++i) {
    if (hostname == servers_[i]->HostName()) {
      return i;
    }
  }
  WHEELS_UNREACHABLE();
}

size_t Network::GetLinkIndex(size_t i, size_t j) const {
  return i * servers_.size() + j;
}

// Partitions

static bool IsSystem(IServer* server) {
  return server->HostName()[0] == 'S';  // TODO
}

static bool Cross(const Link& link, const fault::Partition& lhs) {
  return lhs.count(link.Start()->HostName()) !=
         lhs.count(link.End()->HostName());
}

void Network::PauseLink(const HostName& start, const HostName& end) {
  GlobalAllocatorGuard g;

  LOG_WARN("Pause link {} - {}", start, end);
  GetLink(start, end)->Pause();
}

void Network::ResumeLink(const HostName& start, const HostName& end) {
  GlobalAllocatorGuard g;

  LOG_WARN("Resume link {} - {}", start, end);
  GetLink(start, end)->Resume();
}

void Network::Split(const fault::Partition& lhs) {
  LOG_INFO("Network partitioned: {} / ?", lhs.size());

  for (auto& link : links_) {
    if (link.IsLoopBack()) {
      continue;
    }
    if (!IsSystem(link.Start()) || !IsSystem(link.End())) {
      continue;
    }
    if (Cross(link, lhs)) {
      LOG_WARN("Pause link {} - {}", link.Start()->HostName(),
               link.End()->HostName());
      link.Pause();
    }
  }
}

void Network::Heal() {
  GlobalAllocatorGuard g;

  for (auto& link : links_) {
    link.Resume();
  }
}

}  // namespace whirl::matrix::net
