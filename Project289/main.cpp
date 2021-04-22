#include <Windows.h>
#include <utility>
#include <tuple>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"DirectXTK.lib")
#pragma comment(lib,"DXGI.lib")
#pragma comment(lib,"D3DCompiler.lib")
#pragma comment(lib,"assimp-vc142-mtd.lib")

#include "error_logger.h"
#include "render_window.h"
#include "engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	Engine engine;
	bool can_run = engine.Initialize(
		RenderWindowConfig{}
			.set_hInstance(hInstance)
			.set_window_title("Title")
			.set_window_class("MyTestWindowsClass")
			.set_width(800)
			.set_height(600)
	);
	if (can_run) {
		while (engine.ProcessMessages()) {
			if (!engine.Update()) { break; };
			engine.RenderFrame();
		}
	}
	
	return 0;
}