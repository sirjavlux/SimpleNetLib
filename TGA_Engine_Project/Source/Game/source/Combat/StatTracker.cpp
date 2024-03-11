#include "StatTracker.h"

#include "Packet/PacketManager.h"
#include "../PacketComponents/UpdateStatComponent.hpp"

StatTracker::~StatTracker()
{
	Net::SimpleNetLibCore::Get()->GetPacketComponentDelegator()->UnSubscribeFromPacketComponentDelegate<UpdateStatComponent, StatTracker>(&StatTracker::OnStatUpdateReceived, this);
}

void StatTracker::Initialize()
{
	const PacketComponentAssociatedData associatedDataAckComps = PacketComponentAssociatedData
	{
	false,
	FIXED_UPDATE_DELTA_TIME,
	0.1f,
	EPacketHandlingType::Ack
	};
	
	Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<UpdateStatComponent, StatTracker>(associatedDataAckComps, &StatTracker::OnStatUpdateReceived, this);
}

void StatTracker::SetStat(const uint16_t InPlayerIdentifier, const NetTag& InStatTag, const float InValue)
{
	float& stat = GetStatOrInitializeStat(InPlayerIdentifier, InStatTag.GetHash());
	stat = InValue;

	if (Net::PacketManager::Get()->IsServer())
	{
		UpdateStatComponent component;
		component.stat = stat;
		component.entityIdentifier = InPlayerIdentifier;
		component.statTagHash = InStatTag.GetHash();
		
		Net::PacketManager::Get()->SendPacketComponentMulticast(component);
	}
}

float StatTracker::GetStat(const uint16_t InPlayerIdentifier, const NetTag& InStatTag)
{
	return GetStatOrInitializeStat(InPlayerIdentifier, InStatTag.GetHash());
}

void StatTracker::RemovePlayerStats(const uint16_t InPlayerIdentifier)
{
	if (playerStatMap_.find(InPlayerIdentifier) != playerStatMap_.end())
	{
		playerStatMap_.erase(InPlayerIdentifier);
	}
}

void StatTracker::OnStatUpdateReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent)
{
	const UpdateStatComponent* component = reinterpret_cast<const UpdateStatComponent*>(&InComponent);

	float& statToChange = GetStatOrInitializeStat(component->entityIdentifier, component->statTagHash);
	statToChange = component->stat;
}

void StatTracker::UpdateAllStatsForPlayer(const sockaddr_storage& InAddress) const
{
	if (Net::PacketManager::Get()->IsServer())
	{
		for (const auto& statContainer : playerStatMap_)
		{
			for (const auto& stat : statContainer.second.stats)
			{
				UpdateStatComponent component;
				component.stat = stat.second.value;
				component.entityIdentifier = statContainer.first;
				component.statTagHash = stat.first;
		
				Net::PacketManager::Get()->SendPacketComponent(component, InAddress);
			}
		}
	}
}

float& StatTracker::GetStatOrInitializeStat(const uint16_t InPlayerIdentifier, const uint64_t& InStatTagHash)
{
	if (playerStatMap_.find(InPlayerIdentifier) == playerStatMap_.end())
	{
		playerStatMap_.insert(std::pair<uint16_t, StatContainer>{ InPlayerIdentifier, { InPlayerIdentifier, { }}});
	}
	
	StatContainer& statContainer = playerStatMap_.at(InPlayerIdentifier);
	if (statContainer.stats.find(InStatTagHash) == statContainer.stats.end())
	{
		statContainer.stats.insert(std::pair<uint64_t, Stat>{ InStatTagHash, { InStatTagHash, 0.f } });
	}

	return statContainer.stats.at(InStatTagHash).value;
}
