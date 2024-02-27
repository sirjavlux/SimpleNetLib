#pragma once

#include <memory>

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
	
private:
	std::shared_ptr<BulletManager> bulletManager_;
	
	static Locator* instance_;
};
