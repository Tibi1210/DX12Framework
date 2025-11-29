#include "pch.h"
#include "Application.h"

//#include <windowsx.h>
#include "Renderer/DX12/Debug/DXGIDebug.h"

namespace Engine {

	LRESULT CALLBACK WindProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		
		switch (msg){
			case WM_NCCREATE: {
				LPCREATESTRUCT param = reinterpret_cast<LPCREATESTRUCT>(lParam);
				Application* pointer = reinterpret_cast<Application*>(param->lpCreateParams);
				SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pointer));
				break;
			}
			case WM_CREATE: {
				Application* pointer = reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				pointer->onCreate(hwnd);
				break;
			}
			case WM_DESTROY: {
				Application* pointer = reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				pointer->onDestroy();
				PostQuitMessage(0);
				break;
			}
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	bool Application::Initialize()
	{
		WNDCLASS winInfo = {};
		winInfo.style = NULL;
		winInfo.lpfnWndProc = WindProc;
		winInfo.cbClsExtra = NULL;
		winInfo.cbWndExtra = NULL;
		winInfo.hInstance = NULL;
		winInfo.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		winInfo.hCursor = LoadCursor(NULL, IDC_ARROW);
		winInfo.hbrBackground = (HBRUSH)COLOR_WINDOW;
		winInfo.lpszMenuName = NULL;
		winInfo.lpszClassName = L"BaseWindowClass";

		RegisterClass(&winInfo);

		mWindowHandle = CreateWindowA("BaseWindowClass", "DX12 Window", WS_OVERLAPPEDWINDOW, 200, 200, width, height, NULL, NULL, NULL, this);

		if (!mWindowHandle){
			return false;
		}

		ShowWindow(mWindowHandle, SW_SHOW);
		UpdateWindow(mWindowHandle);

		_isRunning = true;
		return _isRunning;
	}

	void Application::onCreate(HWND hwnd){
		renderer.Initialize(hwnd, width, height);
	}

	void Application::Run(){

		float dt = timeSystem.Tick();

		MSG message;
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)){
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		if (_isRunning)
		{
			renderer.UpdateDraw(dt);
		}
	}

    void Application::onDestroy(){

		_isRunning = false;
		renderer.Release();

		DXGIDebug::Get().GetLiveObjects();

	}
}