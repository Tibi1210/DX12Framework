#pragma once
#include <wrl.h>

namespace Engine {

	class D12DescriptorHeap : public Microsoft::WRL::ComPtr <ID3D12DescriptorHeap>{

	public:

		D12DescriptorHeap() = default;
		~D12DescriptorHeap();

		void InitializeDepthHeap(ID3D12Device* pDevice, const unsigned int numDesc = 0);
		void InitializeCBVSRVUAV(ID3D12Device* pDevice, const unsigned int numDesc);
		void InitializeRTV(ID3D12Device* pDevice, const unsigned int numDesc);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(const size_t idx = 0);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(const size_t idx = 0);

		void Release();

	private:

		UINT incrementSize = 0;
		unsigned int size = 0;


	};
}


