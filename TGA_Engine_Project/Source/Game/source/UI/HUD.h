#pragma once

#include "NetIncludes.h"

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
	void Init();

	void Update();

	void Render();
	
	static ScoreData FetchScoreTextFromNetTarget(const sockaddr_in& InAddress);

private:
	std::vector<ScoreData> statScoreData_;
};
