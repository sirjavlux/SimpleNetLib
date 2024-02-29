#pragma once

#include <memory>

class CollisionManager;
class BulletManager;

class Locator
{
public:
	Locator();
	~Locator();
    
	static Locator* Initialize();
	static Locator* Get();
	static void End();

	BulletManager* GetBulletManager() { return bulletManager_.get(); }
	CollisionManager* GetCollisionManager() { return collisionManager_.get(); }
	
private:
	std::shared_ptr<BulletManager> bulletManager_;
	std::shared_ptr<CollisionManager> collisionManager_;
	
	static Locator* instance_;
};
