#pragma once

#include "EngineBase.h"

#include <Windows.h>

#include "Renderer/Renderer.h"

namespace Engine {

	class RENDER_API Application{

	public:

		Application() = default;

		bool Initialize();
		inline bool IsRunning() { return _isRunning; };

		void onCreate(HWND hwnd);
		void Run();
		void onDestroy();




	private:
		Renderer renderer;
		HWND mWindowHandle = nullptr;
		bool _isRunning = false;

		UINT width = 1280;
		UINT height = 720;
	};

}

