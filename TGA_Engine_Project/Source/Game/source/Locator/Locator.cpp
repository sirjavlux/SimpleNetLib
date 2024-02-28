#include "stdafx.h"

#include "Locator.h"

#include "../Combat/BulletManager.h"

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
	}
}