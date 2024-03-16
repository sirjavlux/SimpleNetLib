#pragma once
#include "stdafx.h"

#include "RenderCamera.h"
#include "EntityComponents/RenderComponent.h"

namespace Net
{
class PacketComponent;
}
class RenderManager
{
public:
	RenderManager();
	~RenderManager();
    
	static RenderManager* Initialize();
	static RenderManager* Get();
	static void End();

	void Render();
	void AddRenderCall(RenderComponent& InRenderComponent);
	
private:
	std::map<RenderData, std::vector<Tga::Sprite2DInstanceData>> renderCallsThisFrame_;
	
	static RenderManager* instance_;
};
