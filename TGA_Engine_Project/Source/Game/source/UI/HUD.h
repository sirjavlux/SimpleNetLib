#pragma once

#include "NetIncludes.h"
#include "tge/text/text.h"

#include <string>

struct ScoreData
{
	std::string username;

	int kills = 0;
	int deaths = 0;

	bool bIsSelf = false;
};

class HUD
{
public:
	~HUD();
	
	void Init();

	void Update();

	void Render();
	
	static ScoreData FetchScoreTextFromNetTarget(const sockaddr_in& InAddress);

private:
	std::vector<ScoreData> statScoreData_;

	Tga::Text* statText_ = nullptr;
};
