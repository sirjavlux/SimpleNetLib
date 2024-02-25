#include "stdafx.h"

#include "RenderManager.h"

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
	// TODO: Render
	
	
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
