#include "CombatComponent.h"

#include "../../Combat/BulletEntity.h"
#include "../../Locator/Locator.h"
#include "../Entities/Entity.h"
#include "../../Combat/StatTracker.h"
#include "../Entities/PlayerShipEntity.h"
#include "Packet/CorePacketComponents/DataReplicationPacketComponent.hpp"

void CombatComponent::Init()
{
	if (Net::PacketManager::Get()->IsServer())
	{
		ColliderComponent* colliderComponent = owner_->GetFirstComponent<ColliderComponent>().lock().get();
		colliderComponent->triggerEnterDelegate.AddDynamic<CombatComponent>(this, &CombatComponent::OnTriggerEntered);
	}
}

void CombatComponent::FixedUpdate()
{
	
}

void CombatComponent::OnTriggerEntered(const ColliderComponent& InColliderComponent)
{
	const CombatComponent* combatComponent = InColliderComponent.GetOwner()->GetFirstComponent<CombatComponent>().lock().get();
	if (combatComponent == nullptr || combatComponent->GetCollisionDamage() < 1)
	{
		return;
	}
	const float collisionDamage = combatComponent->GetCollisionDamage();
	const uint16_t enemyId = InColliderComponent.GetOwner()->GetId();

	if (bIsImmortal_)
	{
		return;
	}

	const BulletEntity* bulletEntity = dynamic_cast<const BulletEntity*>(InColliderComponent.GetOwner());
	if (bulletEntity != nullptr && bulletEntity->GetShooterId() == GetOwner()->GetId())
	{
		return;
	}

	const PlayerShipEntity* playerShip = dynamic_cast<const PlayerShipEntity*>(InColliderComponent.GetOwner());
	if (playerShip != nullptr)
	{
		const BulletEntity* bulletEntitySelf = dynamic_cast<const BulletEntity*>(GetOwner());
		if (bulletEntitySelf != nullptr && bulletEntitySelf->GetShooterId() == playerShip->GetId())
		{
			return;
		}
	}
	
	// If client send client sided events
	if (!Net::PacketManager::Get()->IsServer())
	{
		float currentHealth = Locator::Get()->GetStatTracker()->GetStat(GetId(), NetTag("health")) - collisionDamage;
		currentHealth = currentHealth < 0 ? 0 : currentHealth;
		
		entityTakeDamageDelegate.Execute(enemyId, collisionDamage);
		if (currentHealth <= 0)
		{
			entityDeathDelegate.Execute(enemyId);
		}
		return;
	}

	// Server sided
	Damage(enemyId, collisionDamage);
}

void CombatComponent::OnSendReplication(DataReplicationPacketComponent& OutComponent)
{
	OutComponent.variableDataObject.SerializeMemberVariable(*this, maxHealth_);
	OutComponent.variableDataObject.SerializeMemberVariable(*this, collisionDamage_);
	OutComponent.variableDataObject.SerializeMemberVariable(*this, bIsImmortal_);
}

void CombatComponent::OnReadReplication(const DataReplicationPacketComponent& InComponent)
{
	InComponent.variableDataObject.DeSerializeMemberVariable(*this, maxHealth_);
	InComponent.variableDataObject.DeSerializeMemberVariable(*this, collisionDamage_);
	InComponent.variableDataObject.DeSerializeMemberVariable(*this, bIsImmortal_);
}

void CombatComponent::Damage(const uint16_t InEnemyId, const float InDamage)
{
	float currentHealth = Locator::Get()->GetStatTracker()->GetStat(GetId(), NetTag("health")) - InDamage;
	currentHealth = currentHealth < 0 ? 0 : currentHealth;
	Locator::Get()->GetStatTracker()->SetStat(GetId(), NetTag("health"), currentHealth);

	entityTakeDamageDelegate.Execute(InEnemyId, InDamage);
	if (currentHealth <= 0)
	{
		const float newKillAmount = Locator::Get()->GetStatTracker()->GetStat(InEnemyId, NetTag("kills")) + 1;
		Locator::Get()->GetStatTracker()->SetStat(InEnemyId, NetTag("kills"), newKillAmount);

		const float newDeathAmount = Locator::Get()->GetStatTracker()->GetStat(GetId(), NetTag("deaths")) + 1;
		Locator::Get()->GetStatTracker()->SetStat(GetId(), NetTag("deaths"), newDeathAmount);
		
		entityDeathDelegate.Execute(InEnemyId);
	}
}

void CombatComponent::HealToFullHealth()
{
	Locator::Get()->GetStatTracker()->SetStat(GetId(), NetTag("health"), maxHealth_);
}

void CombatComponent::SetHealth(const float InHealth)
{
	float newHealth = InHealth > maxHealth_ ? maxHealth_ : InHealth;
	newHealth = newHealth < 0 ? 0 : newHealth;
	
	Locator::Get()->GetStatTracker()->SetStat(GetId(), NetTag("health"), newHealth);

	if (newHealth <= 0)
	{
		entityDeathDelegate.Execute(0);
	}
}

float CombatComponent::GetHealth() const
{
	return Locator::Get()->GetStatTracker()->GetStat(GetId(), NetTag("health"));
}
