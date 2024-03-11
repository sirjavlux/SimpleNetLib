#include "stdafx.h"

#include "Locator.h"

#include "../Combat/BulletManager.h"
#include "../Combat/StatTracker.h"
#include "../Entity/Collision/CollisionManager.h"

Locator* Locator::instance_ = nullptr;

Locator::Locator()
{
	
}

Locator::~Locator()
{
	
}

Locator* Locator::Initialize()
{
	if (instance_ == nullptr)
	{
		instance_ = new Locator();
		instance_->bulletManager_ = std::make_shared<BulletManager>();
		instance_->bulletManager_->Initialize();
		instance_->collisionManager_ = std::make_shared<CollisionManager>();
		instance_->collisionManager_->Initialize();
		instance_->statTracker_ = std::make_shared<StatTracker>();
		instance_->statTracker_->Initialize();
	}
	return instance_;
}

Locator* Locator::Get()
{
	return instance_;
}

void Locator::End()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}