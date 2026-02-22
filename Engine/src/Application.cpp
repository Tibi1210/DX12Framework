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
			case WM_ACTIVATE: {
				if (LOWORD(wParam) == WA_INACTIVE)
				{
					//PRINT_N("INACTIVE");
				}
				else
				{
					//PRINT_N("ACTIVE");
				}
				break;
			}
			case WM_ENTERSIZEMOVE: {
				//PRINT_N("RESIZING");
				break;
			}
			case WM_EXITSIZEMOVE: {
				//PRINT_N("RESIZED");
				break;
			}

			case WM_LBUTTONDOWN:{
				//PRINT_N("MOUSE LEFT DOWN");
				break;
			}
			case WM_MBUTTONDOWN: {
				//PRINT_N("MOUSE MIDDLE DOWN");
				break;
			}
			case WM_RBUTTONDOWN: {
				//PRINT_N("MOUSE RIGHT DOWN");
				break;
			}
			case WM_LBUTTONUP: {
				//PRINT_N("MOUSE LEFT UP");
				break;
			}
			case WM_MBUTTONUP: {
				//PRINT_N("MOUSE MIDDLE UP");
				break;
			}
			case WM_RBUTTONUP: {
				//PRINT_N("MOUSE RIGHT UP");
				break;
			}
			case WM_MOUSEMOVE: {
				//PRINT_N("MOUSE MOVED");
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

		SetWindowText(mWindowHandle, L"DX12 app");

		_isRunning = true;
		return _isRunning;
	}

	void Application::onCreate(HWND hwnd){
		renderer.Initialize(hwnd, width, height);
	}

	void Application::Run(){

		MSG message;
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)){
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		if (_isRunning)
		{
			float dt = timeSystem.Tick();
			float gameTime = timeSystem.GetTotalTime();

			CalcualteFrameStats(gameTime);

			renderer.Update(dt, gameTime);
			renderer.Draw();
		}
	}

    void Application::onDestroy(){

		_isRunning = false;
		renderer.Release();

		DXGIDebug::Get().GetLiveObjects();

	}
	void Application::CalcualteFrameStats(const float gameTime){
		static int frameCnt = 0;
		static float timeElapsed = 0.0f;
		frameCnt++;
		if ((gameTime - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCnt;
			float mspf = 1000.0f / fps;
			std::wstringstream ss;
			ss << L"DX12 app, Current FPS: " << fps << L", Frame time: " << mspf;
			std::wstring windowText = ss.str();
			SetWindowText(mWindowHandle, windowText.c_str());
			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;
		}

	}
}