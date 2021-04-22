#include "render_window.h"
#include "window_container.h"

bool RenderWindow::Initialize(WindowContainer& windowContainer, const RenderWindowConfig& cfg) {
	m_hInstance = cfg.hInstance;
	m_width = cfg.width;
	m_height = cfg.height;
	m_window_title = s2w(cfg.window_title);
	m_window_class = s2w(cfg.window_class);

	this->RegisterWindowClass();

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int posX = (GetSystemMetrics(SM_CXSCREEN) - cfg.width) / 2;
	int posY = (GetSystemMetrics(SM_CYSCREEN) - cfg.height) / 2;

	DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	RECT wr;
	wr.left = 0;
	wr.top = 0;
	wr.right = wr.left + cfg.width;
	wr.bottom = wr.top + cfg.height;
	AdjustWindowRect(&wr, style, FALSE);

	m_handle = CreateWindowEx(
		0,
		m_window_class.c_str(),
		m_window_title.c_str(),
		style,
		posX,
		posY,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		m_hInstance,
		&windowContainer
	);

	if (m_handle == NULL) {
		ErrorLogger::Log(GetLastError(), "CreateWindowEx Failed for window: "s + w2s(m_window_title));
		return false;
	}

	ShowWindow(m_handle, SW_SHOW);
	SetForegroundWindow(m_handle);
	SetFocus(m_handle);

	return true;
}

bool RenderWindow::ProcessMessages() {
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	while (PeekMessage(&msg, m_handle, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL) {
		if (!IsWindow(m_handle)) {
			m_handle = NULL;
			UnregisterClass(m_window_class.c_str(), m_hInstance);
			return false;
		}
	}

	return true;
}

RenderWindow::~RenderWindow() {
	if (m_handle != NULL) {
		UnregisterClass(m_window_class.c_str(), m_hInstance);
		DestroyWindow(m_handle);
	}
}

HWND RenderWindow::GetHWND() const {
	return m_handle;
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_CLOSE: {
			DestroyWindow(hwnd);
			return 0;
		}
		default: {
			WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_NCCREATE: {
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
			if (!pWindow) {
				ErrorLogger::Log("Error: pointer to window container is null during WM_NCCREATE.");
				exit(-1);
			}
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));

			return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
		}
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void RenderWindow::RegisterWindowClass() {
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = HandleMessageSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_window_class.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);
}

RenderWindowConfig& RenderWindowConfig::set_hInstance(HINSTANCE hInstance) {
	this->hInstance = hInstance;
	return *this;
}

RenderWindowConfig& RenderWindowConfig::set_window_title(std::string title) {
	this->window_title = std::move(title);
	return *this;
}

RenderWindowConfig& RenderWindowConfig::set_window_class(std::string class_name) {
	this->window_class = std::move(class_name);
	return *this;
}

RenderWindowConfig& RenderWindowConfig::set_width(int width) {
	this->width = width;
	return *this;
}

RenderWindowConfig& RenderWindowConfig::set_height(int height) {
	this->height = height;
	return *this;
}