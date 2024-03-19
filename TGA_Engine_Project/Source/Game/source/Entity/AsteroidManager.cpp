#include "AsteroidManager.h"

#include "EntityManager.h"
#include "RenderManager.h"
#include "../Definitions.hpp"
#include "Collision/CircleCollider.hpp"
#include "Entities/AsteroidEntity.h"
#include "EntityComponents/ColliderComponent.h"

void AsteroidManager::Initialize()
{
	asteroidGenerationData_.push_back(GenerationData{ 2.f, "Sprites/Asteroid/Asteroid0.png", 0.03f });
	asteroidGenerationData_.push_back(GenerationData{ 14.f, "Sprites/Asteroid/Asteroid1.png", 0.02f });
	asteroidGenerationData_.push_back(GenerationData{ 20.f, "Sprites/Asteroid/Asteroid2.png", 0.012f });
	asteroidGenerationData_.push_back(GenerationData{ 5.f, "Sprites/Asteroid/Asteroid3.png", 0.025f });
	

	if (Net::PacketManager::Get()->IsServer())
	{
		constexpr int asteroidsToSpawn = 2000;
		for (int i = 0; i < asteroidsToSpawn; ++i)
		{
			SpawnRandomAsteroidInMap();
		}
	}
}

Entity* AsteroidManager::SpawnAsteroidAtPosition(const Tga::Vector2f& InPosition)
{
	if (!Net::PacketManager::Get()->IsServer())
	{
		return nullptr;
	}
	
	const GenerationData generationData = GetRandomGenerationData(asteroidGenerationData_);
	const NetTag asteroidEntityTag = NetTag("asteroid");

	// Spawn
	AsteroidEntity* entitySpawned = dynamic_cast<AsteroidEntity*>(EntityManager::Get()->SpawnEntityServer(asteroidEntityTag, InPosition));
	if (entitySpawned)
	{
		entitySpawned->SetGenerationData(generationData);
	}
	
	return entitySpawned;
}

void AsteroidManager::SpawnRandomAsteroidInMap()
{
	SpawnAsteroidAtPosition(GetRandomPositionInMap());
}

void AsteroidManager::SpawnRandomAsteroidFromMapBorder()
{
	const Tga::Vector2f position = GetRandomPositionFromMapBorder();
	Entity* entity = SpawnAsteroidAtPosition(position);
	if (!entity)
	{
		return;
	}
	
	// Generate random travel direction
	std::default_random_engine& randEngine = EntityManager::Get()->GetRandomEngine();
	std::uniform_real_distribution distributionX(-1.0f, 1.0f);
	std::uniform_real_distribution distributionY(-1.0f, 1.0f);

	Tga::Vector2f direction;
	direction.x = distributionX(randEngine);
	direction.y = distributionY(randEngine);

	Tga::Vector2f directionToMapCenter = Tga::Vector2f{ 0.f, 0.f } - position;
	directionToMapCenter.Normalize();

	direction += directionToMapCenter;
	direction.Normalize();

	entity->SetDirection(direction);
}

Tga::Vector2f AsteroidManager::GetRandomPositionInMap()
{
	std::default_random_engine& randomEngine = EntityManager::Get()->GetRandomEngine();
  
	std::uniform_real_distribution distributionX(-WORLD_BG_GENERATION_SIZE_X / 2.f, WORLD_BG_GENERATION_SIZE_X / 2.f);
	std::uniform_real_distribution distributionY(-WORLD_BG_GENERATION_SIZE_Y / 2.f, WORLD_BG_GENERATION_SIZE_Y / 2.f);

	const Tga::Vector2f newPosition = { distributionX(randomEngine), distributionY(randomEngine) };

	return newPosition;
}

Tga::Vector2f AsteroidManager::GetRandomPositionFromMapBorder()
{
	std::default_random_engine& randomEngine = EntityManager::Get()->GetRandomEngine();
	
	std::uniform_real_distribution distributionX(-WORLD_BG_GENERATION_SIZE_X / 2.f, WORLD_BG_GENERATION_SIZE_X / 2.f);
	std::uniform_real_distribution distributionY(-WORLD_BG_GENERATION_SIZE_Y / 2.f, WORLD_BG_GENERATION_SIZE_Y / 2.f);
	std::uniform_real_distribution distribution(0.f, 1.f);
	
	const bool bSnapHeight = distribution(randomEngine) >= 0.5f;
	
	Tga::Vector2f newPosition = { distributionX(randomEngine), distributionY(randomEngine) };
	
	if (bSnapHeight)
	{
		newPosition.y = newPosition.y > 0.f ? WORLD_BG_GENERATION_SIZE_Y / 2.f : -WORLD_BG_GENERATION_SIZE_Y / 2.f;
	}
	else
	{
		newPosition.x = newPosition.x > 0.f ? WORLD_BG_GENERATION_SIZE_X / 2.f : -WORLD_BG_GENERATION_SIZE_X / 2.f;
	}

	return newPosition;
}
