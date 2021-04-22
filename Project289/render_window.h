#pragma once

#include <Windows.h>
#include <utility>
#include <memory>

#include "string_utility.h"
#include "error_logger.h"

using namespace std::literals;

class WindowContainer;

class RenderWindowConfig {
public:
	RenderWindowConfig& set_hInstance(HINSTANCE hInstance);
	RenderWindowConfig& set_window_title(std::string title);
	RenderWindowConfig& set_window_class(std::string class_name);
	RenderWindowConfig& set_width(int width);
	RenderWindowConfig& set_height(int height);

	HINSTANCE hInstance;
	std::string window_title;
	std::string window_class;
	int width;
	int height;
};

class RenderWindow {
public:
	RenderWindow() = default;
	RenderWindow(const RenderWindow&) = delete;
	RenderWindow& operator=(const RenderWindow& right) = delete;

	bool Initialize(WindowContainer& windowContainer, const RenderWindowConfig& cfg);
	bool ProcessMessages();
	~RenderWindow();

	HWND GetHWND() const;

private:
	void RegisterWindowClass();

	HWND m_handle = NULL;
	HINSTANCE m_hInstance = NULL;
	std::wstring m_window_title = L"";
	std::wstring m_window_class = L"";
	int m_width = 0;
	int m_height = 0;
};