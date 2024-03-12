#include "AsteroidManager.h"

#include "EntityManager.h"
#include "../Definitions.hpp"

void AsteroidManager::Initialize()
{
	asteroidGenerationData_.push_back(GenerationData{ 1.f, "Sprites/GameBG/Star0.png", 0.04 });
	
}

void AsteroidManager::SpawnRandomAsteroidInMap()
{
	GenerationData generationData = GetRandomGenerationData(asteroidGenerationData_);
}

void AsteroidManager::SpawnRandomAsteroidFromMapBorder()
{
	
}

Tga::Vector2f AsteroidManager::GetRandomPositionInMap()
{
	std::default_random_engine& randomEngine = EntityManager::Get()->GetRandomEngine();
  
	std::uniform_real_distribution distributionX(-WORLD_BG_GENERATION_SIZE_X / 2.1f, WORLD_BG_GENERATION_SIZE_X / 2.1f);
	std::uniform_real_distribution distributionY(-WORLD_BG_GENERATION_SIZE_Y / 2.1f, WORLD_BG_GENERATION_SIZE_Y / 2.1f);

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
