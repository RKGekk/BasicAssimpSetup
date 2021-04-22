#include "engine.h"

bool Engine::Initialize(const RenderWindowConfig& cfg) {
	m_timer.Start();
	if (!m_render_window.Initialize(*this, cfg)) {
		return false;
	}
	if (!m_gfx.Initialize(m_render_window.GetHWND(), cfg.width, cfg.height)) {
		return false;
	}
	return true;
}

bool Engine::ProcessMessages() {
	return m_render_window.ProcessMessages();
}

bool Engine::Update() {
	float dt = m_timer.GetSecondsElapsed();
	m_timer.Restart();

	while (!m_keyboard.CharBufferIsEmpty()) {
		unsigned char ch = m_keyboard.ReadChar();
	}
	while (!m_keyboard.KeyBufferIsEmpty()) {
		KeyboardEvent e = m_keyboard.ReadKey();
	}
	/*while (!m_mouse.EventBufferIsEmpty()) {
		MouseEvent e = m_mouse.ReadEvent();
	}*/

	float mouseSens = 0.005f;
	while (!m_mouse.EventBufferIsEmpty()) {
		MouseEvent e = m_mouse.ReadEvent();
		if (e.GetType() == MouseEvent::EventType::RAW_MOVE && m_mouse.IsRightDown()) {
			m_gfx.m_camera.AdjustRotation(float(e.GetPosY()) * mouseSens, float(e.GetPosX()) * mouseSens, 0.0f);
		}
	}
	float cameraSpeed = 0.75f * dt;
	if (m_keyboard.KeyIsPressed('W')) {
		m_gfx.m_camera.AdjustPosition(DirectX::XMVectorScale(m_gfx.m_camera.GetForward(), cameraSpeed));
	}
	if (m_keyboard.KeyIsPressed('S')) {
		m_gfx.m_camera.AdjustPosition(DirectX::XMVectorScale(m_gfx.m_camera.GetBackward(), cameraSpeed));
	}
	if (m_keyboard.KeyIsPressed('A')) {
		m_gfx.m_camera.AdjustPosition(DirectX::XMVectorScale(m_gfx.m_camera.GetLeft(), cameraSpeed));
	}
	if (m_keyboard.KeyIsPressed('D')) {
		m_gfx.m_camera.AdjustPosition(DirectX::XMVectorScale(m_gfx.m_camera.GetRight(), cameraSpeed));
	}
	if (m_keyboard.KeyIsPressed(VK_SPACE)) {
		m_gfx.m_camera.AdjustPosition(0.0f, cameraSpeed, 0.0f);
	}
	if (m_keyboard.KeyIsPressed('Z')) {
		m_gfx.m_camera.AdjustPosition(0.0f, -cameraSpeed, 0.0f);
	}

	if (m_keyboard.KeyIsPressed(VK_ESCAPE)) {
		return false;
	}

	return true;
}

void Engine::RenderFrame() {
	m_gfx.RenderFrame();
}
