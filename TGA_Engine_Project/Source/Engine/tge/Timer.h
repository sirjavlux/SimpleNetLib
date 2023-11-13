#pragma once
#include <chrono>

class Timer
{
	double timeStart;
	double lastTimestamp;
	float currentDelta;
	
public:
	Timer();
	Timer(const Timer& aTimer) = delete;
	Timer& operator=(const Timer& aTimer) = delete;

	void Update();

	float GetDeltaTime() const;
	double GetTotalTime() const;
};

