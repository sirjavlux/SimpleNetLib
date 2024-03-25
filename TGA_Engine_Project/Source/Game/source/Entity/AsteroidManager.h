#pragma once
#include <random>
#include <vector>

#include "EntityManager.h"
#include "tge/math/Vector2.h"

struct GenerationData
{
	float generationPower = 0.f;
	std::string path;
	
	float colliderSize = 0.01f;
};

class AsteroidManager
{
public:
	void Initialize();

	Entity* SpawnAsteroidAtPosition(const Tga::Vector2f& InPosition);
	
	void SpawnRandomAsteroidInMap();
	void SpawnRandomAsteroidFromMapBorder();

private:
	static Tga::Vector2f GetRandomPositionInMap();
	static Tga::Vector2f GetRandomPositionFromMapBorder();

	std::vector<GenerationData> asteroidGenerationData_;
};

inline GenerationData GetRandomGenerationData(const std::vector<GenerationData>& InGenerationPowerData)
{
	float totalSpriteSelectionPower = 0.f;
	for (const GenerationData& data : InGenerationPowerData)
	{
		totalSpriteSelectionPower += data.generationPower;
	}
	
	std::uniform_real_distribution spriteDistribution(0.f, totalSpriteSelectionPower);
	
	int mapIter = 0;
	float powerIter = 0.f;
	const float randomPower = spriteDistribution(EntityManager::Get()->GetRandomEngine());
	for (const GenerationData& data : InGenerationPowerData)
	{
		++mapIter;
		if (randomPower <= powerIter + data.generationPower)
		{
			return data;
		}
		powerIter += data.generationPower;
	}

	return {};
}