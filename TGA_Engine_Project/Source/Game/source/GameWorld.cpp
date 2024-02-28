#include "stdafx.h"

#include "GameWorld.h"

#include <random>

#include "Entity/EntityManager.h"
#include "Entity/RenderManager.h"
#include "Entity/Entities/PlayerShipEntity.h"
#include "Entity/Entities/SpriteEntity.h"
#include "Definitions.hpp"
#include "Combat/BulletEntity.h"
#include "Combat/BulletManager.h"
#include "Locator/Locator.h"

GameWorld::GameWorld()
{}

GameWorld::~GameWorld()
{
	EntityManager::End();
}

void GameWorld::Init()  
{
	EntityManager::Initialize();
	RenderManager::Initialize();
	Locator::Initialize();
	
	// Register entities
	EntityManager::Get()->RegisterEntityTemplate<PlayerShipEntity>(NetTag("player.ship"));
	EntityManager::Get()->RegisterEntityTemplate<SpriteEntity>(NetTag("sprite"));
	EntityManager::Get()->RegisterEntityTemplate<BulletEntity>(NetTag("bullet"));

	// Generate background stars
	if (Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Client)
	{
		Tga::Engine::GetInstance()->SetClearColor({0, 0, 0});
		GenerateStars();
	}
}

void GameWorld::Update(const float InTimeDelta)
{
	Locator::Get()->GetBulletManager()->Update(InTimeDelta);
	EntityManager::Get()->UpdateEntities(InTimeDelta);
}

void GameWorld::Render()
{
	EntityManager::Get()->RenderEntities();
	RenderManager::Get()->Render();
}


struct BGGenerationData
{
	float generationPower = 0.f;
	std::string path;
};

void GameWorld::GenerateStars() const
{
	constexpr int starsToSpawn = static_cast<int>(WORLD_BG_GENERATION_SIZE_X) * 1200;

	constexpr unsigned int seed = 0;
	std::default_random_engine generator(seed);
    
	// Define a distribution (e.g., uniform distribution between 0 and 99)
	std::uniform_real_distribution distributionX(-WORLD_BG_GENERATION_SIZE_X / 2.f, WORLD_BG_GENERATION_SIZE_X / 2.f);
	std::uniform_real_distribution distributionY(-WORLD_BG_GENERATION_SIZE_Y / 2.f, WORLD_BG_GENERATION_SIZE_Y / 2.f);

	float totalSpriteSelectionPower = 0.f;
	std::vector<BGGenerationData> spritePowerMap;
	spritePowerMap.emplace_back(BGGenerationData{1.f, "Sprites/GameBG/Star0.png"});
	spritePowerMap.emplace_back(BGGenerationData{6.f, "Sprites/GameBG/Star1.png"});
	spritePowerMap.emplace_back(BGGenerationData{1.f, "Sprites/GameBG/Star2.png"});
	spritePowerMap.emplace_back(BGGenerationData{10.f, "Sprites/GameBG/Star3.png"});
	spritePowerMap.emplace_back(BGGenerationData{20.f, "Sprites/GameBG/Star4.png"});
	
	for (const BGGenerationData& pair : spritePowerMap)
	{
		totalSpriteSelectionPower += pair.generationPower;
	}
	
	std::uniform_real_distribution spriteDistribution(0.f, totalSpriteSelectionPower);
	
	for (int i = 0; i < starsToSpawn; ++i)
	{
		const float randomXValue = distributionX(generator);
		const float randomYValue = distributionY(generator);

		Entity* entity = EntityManager::Get()->SpawnEntityLocal(NetTag("sprite"), { randomXValue, randomYValue });
		RenderComponent* renderComponent = entity->GetComponent<RenderComponent>();
		renderComponent->SetSpriteSizeMultiplier(2.f);
		
		// Select Sprite
		int mapIter = 0;
		float powerIter = 0.f;
		const float randomPower = spriteDistribution(generator);
		for (const BGGenerationData& pair : spritePowerMap)
		{
			++mapIter;
			if (randomPower <= powerIter + pair.generationPower)
			{
				renderComponent->SetSpriteTexture(pair.path.c_str());
				
				break;
			}
			powerIter += pair.generationPower;
		}
	}
}
