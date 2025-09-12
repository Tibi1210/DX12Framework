#include "pch.h"
#include "Renderer.h"

#include <vector>

#include "RendererDataTypes.h"

#include "DX12/DXGI/DXGIFactory.h"
#include "DX12/DXGI/DXGIAdapter.h"

#include "DX12/Debug/D12Debug.h"
#include "DX12/Debug/DXGIDebug.h"

namespace Engine {

	using namespace Render;

	Renderer::~Renderer(){
		Release();
	}

	void Renderer::Release() {

		cmdQ.FlushQuene();
		basePipeline.Release();
		dynamicVertexBuffer.Release();
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


		std::vector<Vertex> vertices;
		for (int i = 0; i < 3; i++){

			Vertex vertexData;

			if (i==0){

				vertexData.color = { 1.0f, 0.0f, 0.0f, 1.0f };
				vertexData.position = { -0.5f, -0.5f, 0.0f }; // SS_coords (0, 0) center

			}else if(i==1){

				vertexData.color = { 0.0f, 1.0f, 0.0f, 1.0f };
				vertexData.position = { 0.0f, 0.5f, 0.0f };

			}else{

				vertexData.color = { 0.0f, 0.0f, 1.0f, 1.0f };
				vertexData.position = { 0.5f, -0.5f, 0.0f };

			}
			vertices.push_back(vertexData);
		}


		void* dest = nullptr;
		dynamicVertexBuffer->Map(0, 0, &dest);
		//memcpy(dest, &vertexData, sizeof(Vertex));
		memcpy(dest, vertices.data(), sizeof(Vertex) * vertices.size());
		dynamicVertexBuffer->Unmap(0, 0);
		dynamicVertexBufferView.BufferLocation = dynamicVertexBuffer.Get()->GetGPUVirtualAddress();
		dynamicVertexBufferView.StrideInBytes = sizeof(Vertex);
		dynamicVertexBufferView.SizeInBytes = KBs(16);



		basePipeline.Initialize(device.Get());

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = rWidth;
		viewport.Height = rHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		SRRect.left = 0;
		SRRect.right = viewport.Width;
		SRRect.top = 0;
		SRRect.bottom = viewport.Height;



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

		const float clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapchain.GetCurrentRTVHandle();
		cmdL.GraphicsCmd()->ClearRenderTargetView(rtvHandle, clearColor, 0 ,0);
		cmdL.GraphicsCmd()->OMSetRenderTargets(1, &rtvHandle, false, 0);

		cmdL.GraphicsCmd()->RSSetViewports(1, &viewport);
		cmdL.GraphicsCmd()->RSSetScissorRects(1, &SRRect);

		cmdL.GraphicsCmd()->SetGraphicsRootSignature(basePipeline.GetRootSignature());
		cmdL.GraphicsCmd()->SetPipelineState(basePipeline.Get());
		cmdL.GraphicsCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdL.GraphicsCmd()->IASetVertexBuffers(0, 1, &dynamicVertexBufferView);

		//DRAW
		cmdL.GraphicsCmd()->DrawInstanced(3, 1, 0, 0);


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