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
#include "Entity/AsteroidManager.h"
#include "Entity/Collision/CollisionManager.h"
#include "Entity/Entities/AsteroidEntity.h"
#include "Locator/Locator.h"

GameWorld::GameWorld()
{}

GameWorld::~GameWorld()
{
	EntityManager::End();
	Locator::End();
}

void GameWorld::InitClient()
{
	Tga::Engine::GetInstance()->SetClearColor({0, 0, 0});
	GenerateStars();

	hud_.Init();
}

void GameWorld::Init()  
{
	EntityManager::Initialize();

	// Register entities
	EntityManager::Get()->RegisterEntityTemplate<PlayerShipEntity>(NetTag("player.ship"));
	EntityManager::Get()->RegisterEntityTemplate<SpriteEntity>(NetTag("sprite"));
	EntityManager::Get()->RegisterEntityTemplate<BulletEntity>(NetTag("bullet"));
	EntityManager::Get()->RegisterEntityTemplate<AsteroidEntity>(NetTag("asteroid"));
	
	Locator::Initialize();
}

void GameWorld::Update(const float InTimeDelta)
{
	Locator::Get()->GetCollisionManager()->UpdateComponents();
	Locator::Get()->GetBulletManager()->Update(InTimeDelta);
	
	EntityManager::Get()->UpdateEntities(InTimeDelta);
	
	hud_.Update();
}

void GameWorld::Render()
{
	RenderManager::Get()->Render();
	
	Locator::Get()->GetCollisionManager()->RenderColliderDebugLines();

	hud_.Render();
}

void GameWorld::GenerateStars() const
{
	constexpr int starsToSpawn = static_cast<int>(WORLD_BG_GENERATION_SIZE_X) * 1200;

	constexpr unsigned int seed = 0;
	std::default_random_engine& generator = EntityManager::Get()->GetRandomEngine();
	
	std::uniform_real_distribution distributionX(-WORLD_BG_GENERATION_SIZE_X / 2.f, WORLD_BG_GENERATION_SIZE_X / 2.f);
	std::uniform_real_distribution distributionY(-WORLD_BG_GENERATION_SIZE_Y / 2.f, WORLD_BG_GENERATION_SIZE_Y / 2.f);
	
	std::vector<GenerationData> spritePowerMap;

	// Planets
	constexpr float planetSizeMultiplier = 4.f;
	constexpr float planetSpawnChance = 0.0035f;
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/BlackHole.png", 0.f, 6, 1 });
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/DryDarkPlanet.png", 0.f, planetSizeMultiplier, 2 });
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/Galaxy0.png", 0.f, planetSizeMultiplier, 2 });
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/Galaxy1.png", 0.f, planetSizeMultiplier, 2 });
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/Galaxy2.png", 0.f, planetSizeMultiplier, 2 });
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/GasGiant.png", 0.f, planetSizeMultiplier, 2 });
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/GasGiantRing.png", 0.f, planetSizeMultiplier, 2 });
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/IcePlanet.png", 0.f, planetSizeMultiplier, 2 });
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/LavaPlanet.png", 0.f, planetSizeMultiplier, 2 });
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/Moon.png", 0.f, planetSizeMultiplier, 2 });
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/RedPlanet.png", 0.f, planetSizeMultiplier, 2 });
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/Star.png", 0.f, planetSizeMultiplier, 2 });
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/WaterPlanet.png", 0.f, planetSizeMultiplier, 2 });
	spritePowerMap.emplace_back(GenerationData{planetSpawnChance, "Sprites/Planets/WetPlanet.png", 0.f, planetSizeMultiplier, 2 });

	// Stars far away
	spritePowerMap.emplace_back(GenerationData{1.f, "Sprites/GameBG/Star0.png", 0.f, 2.f, 0 });
	spritePowerMap.emplace_back(GenerationData{6.f, "Sprites/GameBG/Star1.png", 0.f, 2.f, 0 });
	spritePowerMap.emplace_back(GenerationData{1.f, "Sprites/GameBG/Star2.png", 0.f, 2.f, 0 });
	spritePowerMap.emplace_back(GenerationData{10.f, "Sprites/GameBG/Star3.png", 0.f, 2.f, 0 });
	spritePowerMap.emplace_back(GenerationData{20.f, "Sprites/GameBG/Star4.png", 0.f, 2.f, 0 });
	
	for (int i = 0; i < starsToSpawn; ++i)
	{
		const float randomXValue = distributionX(generator);
		const float randomYValue = distributionY(generator);

		Entity* entity = EntityManager::Get()->SpawnEntityLocal(NetTag("sprite"), { randomXValue, randomYValue });
		RenderComponent* renderComponent = entity->GetFirstComponent<RenderComponent>().lock().get();

		entity->SetIsStatic(true);
		entity->SetIsOnlyVisual(true);
		
		// Select Sprite
		const GenerationData generationData = GetRandomGenerationData(spritePowerMap);
		renderComponent->SetSpriteTexture(generationData.path.c_str());

		renderComponent->SetSpriteSizeMultiplier(generationData.sizeMultiplier);
		renderComponent->SetRenderSortingPriority(generationData.sortPriority);
	}
}
