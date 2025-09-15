#pragma once
#include <wrl.h>

namespace Engine {

	class D12Resource : public Microsoft::WRL::ComPtr<ID3D12Resource> {

	public:
		D12Resource() = default;
		~D12Resource();

		D12Resource(const D12Resource& other) = delete;
		D12Resource& operator=(const D12Resource& other) = delete;

		D12Resource(D12Resource&& other) noexcept;
		D12Resource& operator=(D12Resource&& other) noexcept;

		void Initialize(ID3D12Device* pDevice, const unsigned int numBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initState);
		
		void InitializeDepthBuffer(ID3D12Device* pDevice, const unsigned int width, const unsigned int height);

		void* GetCPUMemory();

		void Release();

	private:

		void* memory = nullptr;

	};

}

