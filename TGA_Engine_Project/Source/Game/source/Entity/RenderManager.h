#pragma once
#include "stdafx.h"

#include "RenderCamera.h"
#include "EntityComponents/RenderComponent.h"

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

	RenderCamera& GetCamera() { return camera_; }
	
private:
	std::map<RenderData, std::vector<Tga::Sprite2DInstanceData>> renderCallsThisFrame_;

	RenderCamera camera_;
	
	static RenderManager* instance_;
};
