#include "stdafx.h"

#include "RenderManager.h"

#include "EntityManager.h"
#include "Packet/PacketManager.h"
#include "tge/drawers/SpriteDrawer.h"
#include "tge/graphics/GraphicsEngine.h"

RenderManager* RenderManager::instance_ = nullptr;

RenderManager::RenderManager()
{
	
}

RenderManager::~RenderManager()
{
	
}

RenderManager* RenderManager::Initialize()
{
	if (instance_ == nullptr)
	{
		instance_ = new RenderManager();
	}
	return instance_;
}

RenderManager* RenderManager::Get()
{
	return instance_;
}

void RenderManager::End()
{
	if (instance_ != nullptr)
	{
		delete instance_;
	}
}

void RenderManager::Render()
{
	const auto& engine = *Tga::Engine::GetInstance();
	Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());
	
	// Batch render sprites
	for (const auto& renderCall : renderCallsThisFrame_)
	{
		const RenderData& renderData = renderCall.first;
		Tga::SpriteBatchScope scope = spriteDrawer.BeginBatch(renderData.sharedData);
		for (const Tga::Sprite2DInstanceData& sprite2DInstanceData : renderCall.second)
		{
			scope.Draw(sprite2DInstanceData);
		}
	}
	
	// Clear old calls
	renderCallsThisFrame_.clear();
}

void RenderManager::AddRenderCall(RenderComponent& InRenderComponent)
{
	RenderData renderData = InRenderComponent.GetRenderData();
	if (renderCallsThisFrame_.find(renderData) == renderCallsThisFrame_.end())
	{
		renderCallsThisFrame_.insert({renderData, {}});
	}

	renderCallsThisFrame_.at(renderData).push_back(InRenderComponent.GetSpriteInstanceData());
}
