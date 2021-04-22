#include "window_container.h"

WindowContainer::WindowContainer() {
	static bool raw_input_initialized = false;
	if (!raw_input_initialized) {
		RAWINPUTDEVICE rid;
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = 0;
		if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
			ErrorLogger::Log(GetLastError(), "Failed to register raw input device.");
			exit(-1);
		}
		raw_input_initialized = true;
	}
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT WindowContainer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) { return true; }

	switch (uMsg) {
		case WM_CHAR: {
			unsigned char letter = static_cast<unsigned char>(wParam);
			if (m_keyboard.IsCharAutoRepeat()) {
				m_keyboard.OnChar(letter);
			}
			else {
				const bool wasPressed = lParam & 0x40000000;
				if (!wasPressed) {
					m_keyboard.OnChar(letter);
				}
			}
			return 0;
		}
		case WM_KEYDOWN: {
			unsigned char keycode = static_cast<unsigned char>(wParam);
			if (m_keyboard.IsKeyAutoRepeat()) {
				m_keyboard.OnKeyPressed(keycode);
			}
			else {
				const bool wasPressed = lParam & 0x40000000;
				if (!wasPressed) {
					m_keyboard.OnKeyPressed(keycode);
				}
			}
			return 0;
		}
		case WM_KEYUP: {
			unsigned char keycode = static_cast<unsigned char>(wParam);
			m_keyboard.OnKeyReleased(keycode);
			return 0;
		}
		case WM_MOUSEMOVE: {
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			m_mouse.OnMouseMove(x, y);
			return 0;
		}
		case WM_LBUTTONDOWN: {
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			m_mouse.OnLeftPressed(x, y);
			return 0;
		}
		case WM_RBUTTONDOWN: {
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			m_mouse.OnRightPressed(x, y);
			return 0;
		}
		case WM_MBUTTONDOWN: {
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			m_mouse.OnMiddlePressed(x, y);
			return 0;
		}
		case WM_LBUTTONUP: {
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			m_mouse.OnLeftReleased(x, y);
			return 0;
		}
		case WM_RBUTTONUP: {
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			m_mouse.OnRightReleased(x, y);
			return 0;
		}
		case WM_MBUTTONUP: {
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			m_mouse.OnMiddleReleased(x, y);
			return 0;
		}
		case WM_MOUSEWHEEL: {
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				m_mouse.OnWheelUp(x, y);
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
				m_mouse.OnWheelDown(x, y);
			}
			return 0;
		}
		case WM_INPUT : {
			UINT data_size = 0u;
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &data_size, sizeof(RAWINPUTHEADER));
			if (data_size) {
				UINT data_size_old = data_size;
				std::unique_ptr<BYTE[]> raw_data = std::make_unique<BYTE[]>(data_size);
				data_size = GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, raw_data.get(), &data_size, sizeof(RAWINPUTHEADER));
				if (data_size == data_size_old) {
					RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(raw_data.get());
					if (raw->header.dwType == RIM_TYPEMOUSE) {
						m_mouse.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
					}
				}
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
