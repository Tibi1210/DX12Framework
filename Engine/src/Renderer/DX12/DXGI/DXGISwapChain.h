#pragma once
#include <wrl.h>
#include "EngineSettings.h"

#include<d3d12.h>
#include<dxgi1_6.h>

namespace Engine {

	class RENDER_API DXGISwapChain : public Microsoft::WRL::ComPtr<IDXGISwapChain1>{

	public:
		DXGISwapChain() = default;
		~DXGISwapChain();


		void Initialize(ID3D12Device* pDevice, IDXGIFactory2* pFactory, ID3D12CommandQueue* pCmdQ, const HWND hwnd, const UINT width, const UINT height);
		void CreateBuffers();
		void DropBuffers();

		void Present();

		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle();
		inline ID3D12Resource* GetCurrentRT() { return renderTargets[currentBuffer].Get();}

		void Release();

	private:

		Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[G_MAX_SWAPCHAIN_BUFFERS];
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;

		UINT bufferCount = 0;
		UINT currentBuffer = 0;

		UINT sWidth, sHeight = 0;
		UINT allocSize = 0;
		UINT heapIncrement = 0;

		ID3D12Device* device = nullptr;

	};
}

