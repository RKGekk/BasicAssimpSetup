#pragma once

#include "window_container.h"
#include "timer.h"

class Engine : public WindowContainer {
public:
	bool Initialize(const RenderWindowConfig& cfg);
	bool ProcessMessages();
	bool Update();
	void RenderFrame();

private:
	Timer m_timer;
};