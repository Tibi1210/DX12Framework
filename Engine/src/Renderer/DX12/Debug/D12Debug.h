#pragma once
#include <wrl.h>

namespace Engine {

	class D12Debug : public Microsoft::WRL::ComPtr<ID3D12Debug>{
	public:
		D12Debug() = default;

		inline static D12Debug& Get() { return instance; }

		void Enable();


	private:

		static D12Debug instance;

		void GetInterface();

	};
}

