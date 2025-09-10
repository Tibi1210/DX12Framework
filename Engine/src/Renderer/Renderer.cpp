#include "pch.h"
#include "Renderer.h"

#include "RendererDataTypes.h"

#include "DX12/DXGI/DXGIFactory.h"
#include "DX12/DXGI/DXGIAdapter.h"

#include "DX12/Debug/D12Debug.h"
#include "DX12/Debug/DXGIDebug.h"

#include "DX12/Pipeline/HLSLShader.h"
#include "DX12/Pipeline/D12RootSignature.h"

namespace Engine {

	using namespace Render;

	Renderer::~Renderer(){
		Release();
	}

	void Renderer::Release() {
		
		dynamicVertexBuffer.Release();
		cmdQ.FlushQuene();
		swapchain.Release();
		cmdL.Release();
		cmdQ.Release();

		if (device.Get()) {
			device.Reset();
		}
	}


	void Renderer::Initialize(HWND hwnd, const UINT width, const UINT height){

		rWidth = width;
		rHeight = height;
		// disable on prod
		D12Debug::Get().Enable();
		DXGIDebug::Get().Enable();

		DXGIFactory factory;
		DXGIAdapter adapter = factory.GetAdapter();

		device.Initialize(adapter.Get());

		cmdQ.Initialize(device.Get());
		cmdL.Initialize(device.Get());
		
		swapchain.Initialize(device.Get(), factory.Get(), cmdQ.Get(), hwnd, rWidth, rHeight);

		dynamicVertexBuffer.Initialize(device.Get(), KBs(16), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
		dynamicVertexBuffer.Get()->SetName(L"Dynamic vertex buffer");

		Vertex vertexData;
		vertexData.position = { 1.0f, 5.0f, 3.0f };
		vertexData.color = { 1.0f, 0.0f, 0.0f, 1.0f };
		void* dest = nullptr;
		dynamicVertexBuffer->Map(0, 0, &dest);
		memcpy(dest, &vertexData, sizeof(vertexData));
		dynamicVertexBuffer->Unmap(0, 0);



		HLSLShader testShader;
		testShader.Initialize(L"Shaders/vertex.hlsl", HLSLShader::ShaderType::VERTEX);

		D12RootSignature rstest;
		rstest.Initialize(device.Get());









	}

	void Renderer::UpdateDraw(){

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = swapchain.GetCurrentRT();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		cmdL.GraphicsCmd()->ResourceBarrier(1, &barrier);

		const float clearColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapchain.GetCurrentRTVHandle();
		cmdL.GraphicsCmd()->ClearRenderTargetView(rtvHandle, clearColor, 0 ,0);

		barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = swapchain.GetCurrentRT();
		barrier.Transition.Subresource = 0; 
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		cmdL.GraphicsCmd()->ResourceBarrier(1, &barrier);

		cmdL.GraphicsCmd()->Close();
		cmdQ.ExecuteCmdList(cmdL.Get());

		swapchain.Present();

		while (cmdQ.GetFence()->GetCompletedValue() < cmdQ.M_GetCurrentFence()){
			_mm_pause();
		}

		cmdL.ResetCmd();
	}
}