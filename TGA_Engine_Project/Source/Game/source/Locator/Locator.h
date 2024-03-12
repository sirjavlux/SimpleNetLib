#pragma once

#include <memory>

class AsteroidManager;
class StatTracker;
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
	StatTracker* GetStatTracker() { return statTracker_.get(); }
	AsteroidManager* GetAsteroidManager() { return asteroidManager_.get(); }
	
private:
	std::shared_ptr<BulletManager> bulletManager_;
	std::shared_ptr<CollisionManager> collisionManager_;
	std::shared_ptr<StatTracker> statTracker_;
	std::shared_ptr<AsteroidManager> asteroidManager_;
	
	static Locator* instance_;
};
