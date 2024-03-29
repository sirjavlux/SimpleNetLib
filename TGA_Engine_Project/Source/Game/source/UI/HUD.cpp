#include "HUD.h"

#include <iomanip>

#include "../Combat/StatTracker.h"
#include "../Entity/EntityManager.h"
#include "../Entity/Entities/PlayerShipEntity.h"
#include "../Locator/Locator.h"
#include "../Definitions.hpp"

HUD::~HUD()
{
	delete statText_;
}

void HUD::Init()
{
	if (!Net::PacketManager::Get()->IsServer())
	{
		statText_ = new Tga::Text(L"Text/arial.ttf", Tga::FontSize_18);
	}
}

void HUD::Update()
{
	if (Net::PacketManager::Get()->IsServer())
	{
		return;
	}
	
	statScoreData_.clear();
	
	const std::unordered_map<sockaddr_in, uint16_t>& possessionMap = EntityManager::Get()->GetEntityPossessionMap();
	for (const auto& possession : possessionMap)
	{
		const ScoreData scoreData = FetchScoreTextFromNetTarget(possession.first);
		statScoreData_.push_back(scoreData);
	}
}

void HUD::Render()
{
	if (Net::PacketManager::Get()->IsServer())
	{
		return;
	}
	
	auto& engine = *Tga::Engine::GetInstance();
	const Tga::Vector2ui intResolution = engine.GetRenderSize();
	const Tga::Vector2f anchorPoint = { 0.34f, 0.2f };
	constexpr float yIncrementPerText = -0.015f;
	
	const float xOffset = static_cast<float>(intResolution.x) / 2;
	const float yOffset = static_cast<float>(intResolution.y) / 2;
	
	Tga::Vector2f position = anchorPoint;
	for (const ScoreData& data : statScoreData_)
	{
		// Username
		statText_->SetText(data.username);
		statText_->SetPosition(Tga::Vector2f{ position.x * static_cast<float>(intResolution.x),
			position.y * static_cast<float>(intResolution.x) } + Tga::Vector2f{ xOffset, yOffset });
		if (data.bIsSelf)
		{
			statText_->SetColor(Tga::Color(0.4f, 0.4f, 1));
		} else
		{
			statText_->SetColor(Tga::Color(1, 1, 1));
		}
		statText_->Render();

		// Kills
		statText_->SetText(std::to_string(data.kills));
		const float killsTextSize = statText_->GetWidth();
		statText_->SetPosition(Tga::Vector2f{ (position.x + 0.1f) * static_cast<float>(intResolution.x),
			position.y * static_cast<float>(intResolution.x) } + Tga::Vector2f{ xOffset, yOffset });
		statText_->SetColor(Tga::Color(0, 1, 0));
		statText_->Render();

		// Middle spacing
		statText_->SetText(":");
		statText_->SetPosition(Tga::Vector2f{ (position.x + 0.103f) * static_cast<float>(intResolution.x) + killsTextSize,
			position.y * static_cast<float>(intResolution.x) } + Tga::Vector2f{ xOffset, yOffset });
		statText_->SetColor(Tga::Color(1, 1, 1));
		statText_->Render();
		
		// Deaths
		statText_->SetText(std::to_string(data.deaths));
		statText_->SetPosition(Tga::Vector2f{ (position.x + 0.11f) * static_cast<float>(intResolution.x) + killsTextSize,
			position.y * static_cast<float>(intResolution.x) } + Tga::Vector2f{ xOffset, yOffset });
		statText_->SetColor(Tga::Color(1, 0, 0));
		statText_->Render();
		
		position.y += yIncrementPerText;
	}
}

ScoreData HUD::FetchScoreTextFromNetTarget(const sockaddr_in& InAddress)
{
	ScoreData result;

	const std::unordered_map<sockaddr_in, uint16_t>& possessionMap = EntityManager::Get()->GetEntityPossessionMap();
	if (possessionMap.find(InAddress) != possessionMap.end())
	{
		const PlayerShipEntity* player = dynamic_cast<PlayerShipEntity*>(EntityManager::Get()->GetEntityById(possessionMap.at(InAddress)));
		if (player != nullptr && EntityManager::Get()->GetPossessedEntity() != nullptr)
		{
			result.bIsSelf = EntityManager::Get()->GetPossessedEntity()->GetId() == player->GetId();
			
			const uint16_t entityId = player->GetId();
			
			StatTracker* statTracker = Locator::Get()->GetStatTracker();
			const int kills = static_cast<int>(statTracker->GetStat(entityId, NetTag("stat.kills")));
			const int deaths = static_cast<int>(statTracker->GetStat(entityId, NetTag("stat.deaths")));
			
			std::string username = player->GetUsername().ToStr();
			
			username = username.substr(0, USERNAME_MAX_LENGTH);
			
			result.username = username;
			result.kills = kills;
			result.deaths = deaths;
		}
	}

	return result;
}
