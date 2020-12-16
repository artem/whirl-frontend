#include <whirl/matrix/network/network.hpp>

#include <whirl/matrix/memory/allocator.hpp>
#include <whirl/matrix/world/global/random.hpp>

#include <wheels/support/assert.hpp>
#include <wheels/support/compiler.hpp>

namespace whirl::net {

void Network::AddServer(INetServer* server) {
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
  digest_.Eat(packet.source_port)
      .Eat(packet.dest_port)
      .Eat(packet.message.length());

  link->End()->HandlePacket(packet, link->GetOpposite());
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

static bool IsClient(INetServer* server) {
  return server->HostName()[0] == 'C';  // TODO
}

static bool Cross(const Link& link, const Partition& lhs) {
  return lhs.count(link.StartHostName()) != lhs.count(link.EndHostName());
}

void Network::Split() {
  GlobalHeapScope g;

  std::vector<std::string> servers;

  // Exclude clients

  for (INetServer* server : servers_) {
    if (!IsClient(server)) {
      servers.push_back(server->HostName());
    }
  }

  // Generate random partition

  // [1, servers.size())
  size_t lhs_size = GlobalRandomNumber(1, servers.size());

  Partition lhs;
  for (auto& hostname : GlobalRandomSelect(servers, lhs_size)) {
    lhs.insert(hostname);
  }

  // Print
  WHIRL_SIM_LOG("Network partitioned: {} / {}", lhs.size(),
                servers.size() - lhs.size());

  // Split

  for (auto& link : links_) {
    if (link.IsLoopBack()) {
      continue;
    }
    if (IsClient(link.Start()) || IsClient(link.End())) {
      continue;
    }
    if (Cross(link, lhs)) {
      WHIRL_SIM_LOG_WARN("Pause link {} - {}", link.StartHostName(), link.EndHostName());
      link.Pause();
    }
  }
}

void Network::Split(const Partition& lhs) {
  for (auto& link : links_) {
    if (Cross(link, lhs)) {
      link.Pause();
    }
  }
}

void Network::Heal() {
  GlobalHeapScope g;

  for (auto& link : links_) {
    link.Resume();
  }
}

}  // namespace whirl::net
