#pragma once
#include <chrono>

class FrameTimer
{
public:
	FrameTimer();									// intialize last
	float Mark();									// get duration from now to last and reset last
	float Peek() const;								// get the time from last without resetting last
private:
	std::chrono::steady_clock::time_point last;		// time point
};
