#pragma once

#include <chrono>

class Timer {
public:
	Timer();
	float GetMilisecondsElapsed();
	float GetSecondsElapsed();
	void Restart();
	bool Stop();
	bool Start();

private:
	bool m_is_running = false;

	std::chrono::time_point<std::chrono::steady_clock> m_start;
	std::chrono::time_point<std::chrono::steady_clock> m_stop;
};