#include "timer.h"

Timer::Timer() : m_start(std::chrono::steady_clock::now()), m_stop(std::chrono::steady_clock::now()) {}

float Timer::GetMilisecondsElapsed() {
	if (m_is_running) {
		auto elapsed = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - m_start);
		return elapsed.count();
	}
	else {
		auto elapsed = std::chrono::duration<float, std::milli>(m_stop - m_start);
		return elapsed.count();
	}
}

float Timer::GetSecondsElapsed() {
	if (m_is_running) {
		auto elapsed = std::chrono::duration<float>(std::chrono::steady_clock::now() - m_start);
		return elapsed.count();
	}
	else {
		auto elapsed = std::chrono::duration<float>(m_stop - m_start);
		return elapsed.count();
	}
}

void Timer::Restart() {
	m_is_running = true;
	m_start = std::chrono::steady_clock::now();
}

bool Timer::Stop() {
	if (!m_is_running) {
		return false;
	}
	else {
		m_stop = std::chrono::steady_clock::now();
		m_is_running = false;
		return true;
	}
}

bool Timer::Start() {
	if (m_is_running) {
		return false;
	}
	else {
		m_start = std::chrono::steady_clock::now();
		m_is_running = true;
		return true;
	}
}
