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
	Locator::Initialize();
	
	// Register entities
	EntityManager::Get()->RegisterEntityTemplate<PlayerShipEntity>(NetTag("player.ship"));
	EntityManager::Get()->RegisterEntityTemplate<SpriteEntity>(NetTag("sprite"));
	EntityManager::Get()->RegisterEntityTemplate<BulletEntity>(NetTag("bullet"));
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
	EntityManager::Get()->RenderEntities();
	
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
	spritePowerMap.emplace_back(GenerationData{1.f, "Sprites/GameBG/Star0.png"});
	spritePowerMap.emplace_back(GenerationData{6.f, "Sprites/GameBG/Star1.png"});
	spritePowerMap.emplace_back(GenerationData{1.f, "Sprites/GameBG/Star2.png"});
	spritePowerMap.emplace_back(GenerationData{10.f, "Sprites/GameBG/Star3.png"});
	spritePowerMap.emplace_back(GenerationData{20.f, "Sprites/GameBG/Star4.png"});
	
	for (int i = 0; i < starsToSpawn; ++i)
	{
		const float randomXValue = distributionX(generator);
		const float randomYValue = distributionY(generator);

		Entity* entity = EntityManager::Get()->SpawnEntityLocal(NetTag("sprite"), { randomXValue, randomYValue });
		RenderComponent* renderComponent = entity->GetFirstComponent<RenderComponent>().lock().get();
		renderComponent->SetSpriteSizeMultiplier(2.f);
		
		// Select Sprite
		const GenerationData generationData = GetRandomGenerationData(spritePowerMap);
		renderComponent->SetSpriteTexture(generationData.path.c_str());
	}
}
