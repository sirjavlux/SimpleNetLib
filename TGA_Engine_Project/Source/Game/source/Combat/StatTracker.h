#pragma once

#include <cstdint>

#include "NetIncludes.h"
#include "Packet/PacketComponent.h"
#include "Utility/NetTag.h"

struct Stat
{
	uint64_t hashStatTag;
	
	float value;
};

struct StatContainer
{
	uint16_t owningPlayerIdentifier;
	
	std::unordered_map<uint64_t, Stat> stats;
};

class StatTracker
{
public:
	~StatTracker();
	void Initialize();
	
	void SetStat(uint16_t InPlayerIdentifier, const NetTag& InStatTag, float InValue);
	float GetStat(uint16_t InPlayerIdentifier, const NetTag& InStatTag);

	void RemovePlayerStats(uint16_t InPlayerIdentifier);

	void OnStatUpdateReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);

	void UpdateAllStatsForPlayer(const sockaddr_storage& InAddress) const;
	
private:
	float& GetStatOrInitializeStat(uint16_t InPlayerIdentifier, const uint64_t& InStatTagHash);
	
	std::unordered_map<uint16_t, StatContainer> playerStatMap_;
};
