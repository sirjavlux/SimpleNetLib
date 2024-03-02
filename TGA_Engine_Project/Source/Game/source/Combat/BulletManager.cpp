#include "stdafx.h"

#include "BulletManager.h"

#include "BulletEntity.h"
#include "../Entity/EntityManager.h"
#include "Packet/PacketManager.h"

void BulletManager::Initialize()
{
	
}

void BulletManager::Update(const float InDeltaTime)
{
	
}

void BulletManager::TryShootBulletServer(const uint16_t InEntityIdentifier)
{
	using namespace std::chrono;
	
	if (lastTimeBulletShotMap_.find(InEntityIdentifier) == lastTimeBulletShotMap_.end())
	{
		lastTimeBulletShotMap_.insert({ InEntityIdentifier, steady_clock::now() });
	}
	else
	{
		steady_clock::time_point& lastTimePoint = lastTimeBulletShotMap_.at(InEntityIdentifier);
		const steady_clock::time_point currentTime = steady_clock::now();
		const duration<double> timeSinceLastBullet = duration_cast<duration<double>>(currentTime - lastTimePoint);
		if (timeSinceLastBullet.count() < bulletShootDelay_)
		{
			return;
		}
		lastTimePoint = steady_clock::now();
	}

	ShootBullet(InEntityIdentifier);
}

void BulletManager::ShootBullet(const uint16_t InEntityIdentifier)
{
	const NetTag bulletEntityTag = NetTag("bullet");
	const Entity* entityShooter = EntityManager::Get()->GetEntityById(InEntityIdentifier);
	if (entityShooter == nullptr)
	{
		return;
	}
	
	BulletEntity* entitySpawned = dynamic_cast<BulletEntity*>(EntityManager::Get()->SpawnEntityServer(bulletEntityTag, entityShooter->GetPosition(), entityShooter->GetDirection()));
	entitySpawned->SetShooter(InEntityIdentifier);
}