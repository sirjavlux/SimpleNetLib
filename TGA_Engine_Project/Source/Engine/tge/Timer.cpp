#include "stdafx.h"

#include "Timer.h"

#include <chrono>

Timer::Timer()
{
	using namespace std::chrono;

	double startMs = static_cast<double>(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count());
	timeStart = startMs;
	lastTimestamp = startMs;
}

void Timer::Update()
{
	using namespace std::chrono;
	double newTimestamp = static_cast<double>(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count());

	currentDelta = static_cast<float>((newTimestamp - lastTimestamp) / 1000);
	lastTimestamp = newTimestamp;
}

float Timer::GetDeltaTime() const
{
	return currentDelta;
}

double Timer::GetTotalTime() const
{
	return static_cast<double>((lastTimestamp - timeStart) / 1000);
}
