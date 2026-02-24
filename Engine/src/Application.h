#pragma once

#include "EngineBase.h"

#include<Windows.h>
#include<Windowsx.h>

#include "Renderer/Renderer.h"

#include "Renderer/DX12/Time/TimeStep.h"

namespace Engine {

	class RENDER_API Application{

	public:

		Application() = default;

		bool Initialize();
		inline bool IsRunning() { return _isRunning; };

		void onCreate(HWND hwnd);
		void Run();
		void onDestroy();

		void CalcualteFrameStats(const float gameTime);

		void getRawInput(const int event, const int x, const int y);


	private:
		Renderer renderer;
		Timer::TimeStep timeSystem;

		HWND mWindowHandle = nullptr;
		bool _isRunning = false;

		UINT width = 1280;
		UINT height = 720;

	};

}

