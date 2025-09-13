#pragma once

#include "EngineBase.h"
#include <Windows.h>

#include<DirectXMath.h>

#include "RendererDataTypes.h"

#include "DX12/DXGI/DXGISwapChain.h"

#include "DX12/Commands/D12CmdQueue.h"
#include "DX12/Commands/D12CmdList.h"

#include "DX12/Base/D12Device.h"

#include "DX12/Resources/D12Resource.h"

#include "DX12/Pipeline/D12PipelineState.h"
#include "DX12/Descriptors/D12DescriptorHeap.h"
#include "DX12/MemoryManagement/BufferUploader.h"

namespace Engine {

	class RENDER_API Renderer{

	public:
		Renderer() = default;
		~Renderer();

		void Initialize(HWND hwnd, const UINT width, const UINT height);

		void UpdateDraw();


		void Release();

	private:

		UINT rWidth, rHeight = 0;
		D3D12_VIEWPORT viewport;
		D3D12_RECT SRRect;


		D12Device device;
		D12CmdQueue cmdQ;
		D12CmdList cmdL;
		DXGISwapChain swapchain;
		BufferUploader bufferUploader;

		D12Resource vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		D12Resource indexBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;

		D12Resource materialBuffer1;

		D12PipelineState basePipeline;
		D12Resource depthBuffer;
		D12DescriptorHeap depthHeap;
		D12Resource PassDataBuffer;

		Render::Light lights[8];

		DirectX::XMMATRIX viewProjMatrix;



	};
}

