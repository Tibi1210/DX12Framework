#include "pch.h"
#include "DXGISwapChain.h"


namespace Engine {
	DXGISwapChain::~DXGISwapChain(){
		Release();
	}

	void DXGISwapChain::Initialize(ID3D12Device* pDevice, IDXGIFactory2* pFactory, ID3D12CommandQueue* pCmdQ, const HWND hwnd, const UINT width, const UINT height){

		sWidth = width;
		sHeight = height;

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = G_MAX_SWAPCHAIN_BUFFERS;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		EVAL_HRES(pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(rtvHeap.GetAddressOf())), "Error creating RTV heap.");
		heapIncrement = pDevice->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);

		DXGI_SWAP_CHAIN_DESC1 scDesc = {};
		scDesc.Width = sWidth;
		scDesc.Height = sHeight;
		scDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		scDesc.Stereo = false;
		scDesc.SampleDesc = {1,0};
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.BufferCount = G_MAX_SWAPCHAIN_BUFFERS;
		scDesc.Scaling = DXGI_SCALING_NONE;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		EVAL_HRES(pFactory->CreateSwapChainForHwnd(pCmdQ, hwnd, &scDesc, nullptr, nullptr, &ptr_), "Error creating swapchain.");
		
		bufferCount = G_MAX_SWAPCHAIN_BUFFERS;
		device = pDevice;

		CreateBuffers();

	}

	void DXGISwapChain::CreateBuffers(){

		if (!Get()){
			PRINT_N("Error: tried to create buffers for uninitialized swapchain.");
			return;
		}

		for (int i = 0; i < bufferCount; i++){
			renderTargets[i].Reset();
			EVAL_HRES(Get()->GetBuffer(i, IID_PPV_ARGS(renderTargets[i].GetAddressOf())), "Error while creating buffer for swapchain idx: " << i);
			
			D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
			cpuDescHandle.ptr += (size_t)heapIncrement * i;

			device->CreateRenderTargetView(renderTargets[i].Get(), 0, cpuDescHandle);
		}

	}

	void DXGISwapChain::DropBuffers(){

		for (int i = 0; i < bufferCount; i++) {
			renderTargets[i].Reset();
		}

	}

	void DXGISwapChain::Present(){

		Get()->Present(0, 0);

		currentBuffer = (currentBuffer + 1) % bufferCount;

	}

	D3D12_CPU_DESCRIPTOR_HANDLE DXGISwapChain::GetCurrentRTVHandle(){

		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		cpuDescHandle.ptr += (size_t)currentBuffer * heapIncrement;

		return cpuDescHandle;
	}

	void DXGISwapChain::Release(){
		if (Get())
		{
			DropBuffers();
			rtvHeap.Reset();
			Reset();
		}
	}
}