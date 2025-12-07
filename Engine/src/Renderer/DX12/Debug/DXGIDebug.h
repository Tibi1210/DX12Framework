#pragma once
#include <wrl.h>

namespace Engine {

	class DXGIDebug : public Microsoft::WRL::ComPtr<IDXGIDebug> {
	public:
		DXGIDebug() = default;

		inline static DXGIDebug& Get() { return instance; }

		void Enable();
		void GetLiveObjects();


	private:

		static DXGIDebug instance;

		void GetInterface();

	};
}

