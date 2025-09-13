#include "pch.h"
#include "Renderer.h"

#include <vector>

#include "DX12/DXGI/DXGIFactory.h"
#include "DX12/DXGI/DXGIAdapter.h"

#include "DX12/Debug/D12Debug.h"
#include "DX12/Debug/DXGIDebug.h"

#include "DX12/Utilities/Utilities.h"

namespace Engine {

	using namespace Render;

	Renderer::~Renderer(){
		Release();
	}

	void Renderer::Release() {

		cmdQ.FlushQuene();



		materialBuffer1.Release();
		PassDataBuffer.Release();
		depthHeap.Release();
		depthBuffer.Release();
		basePipeline.Release();
		indexBuffer.Release();
		vertexBuffer.Release();
		bufferUploader.Release();
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
		bufferUploader.Initialize(device.Get(), KBs(64));
		vertexBuffer.Initialize(device.Get(), KBs(8), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
		vertexBuffer.Get()->SetName(L"Vertex buffer");

		#define G_VERTS 24
		Vertex boxVerts[G_VERTS];

		boxVerts[0].position = { -1.0f, 1.0f, 1.0f };
		boxVerts[0].normal = { 0.0f, 1.0f, 0.0f };

		boxVerts[1].position = { -1.0f, 1.0f, -1.0f };
		boxVerts[1].normal = { 0.0f, 1.0f, 0.0f };

		boxVerts[2].position = { 1.0f, 1.0f, 1.0f };
		boxVerts[2].normal = { 0.0f, 1.0f, 0.0f };

		boxVerts[3].position = { -1.0f, -1.0f, -1.0f };
		boxVerts[3].normal = { 0.0f, 0.0f, -1.0f };

		boxVerts[4].position = { 1.0f, -1.0f, -1.0f };
		boxVerts[4].normal = { 0.0f, 0.0f, -1.0f };

		boxVerts[5].position = { -1.0f, 1.0f, -1.0f };
		boxVerts[5].normal = { 0.0f, 0.0f, -1.0f };

		boxVerts[6].position = { 1.0f, -1.0f, -1.0f };
		boxVerts[6].normal = { 1.0f, 0.0f, 0.0f };

		boxVerts[7].position = { 1.0f, -1.0f, 1.00000 };
		boxVerts[7].normal = { 1.0f, 0.0f ,0.0f };

		boxVerts[8].position = { 1.0f, 1.0f, -1.0f };
		boxVerts[8].normal = { 1.0f, 0.0f, 0.0f };

		boxVerts[9].position = { 1.0f, -1.0f, 1.0f };
		boxVerts[9].normal = { 0.0f, -1.0f, 0.0f };

		boxVerts[10].position = { 1.0f, -1.0f, -1.0f };
		boxVerts[10].normal = { 0.0f, -1.0f, 0.0f };

		boxVerts[11].position = { -1.0f, -1.0f, 1.0f };
		boxVerts[11].normal = { 0.0f, -1.0f, 0.0f };

		boxVerts[12].position = { -1.0f, -1.0f, 1.0f };
		boxVerts[12].normal = { -1.0f, 0.0f ,0.0f };

		boxVerts[13].position = { -1.0f, -1.0f, -1.0f };
		boxVerts[13].normal = { -1.0f, 0.0f, 0.0f };

		boxVerts[14].position = { -1.0f, 1.0f, 1.0f };
		boxVerts[14].normal = { -1.0f, 0.0f, 0.0f };

		boxVerts[15].position = { 1.0f, -1.0f, 1.00000 };
		boxVerts[15].normal = { 0.0f, 0.0f ,1.0f };

		boxVerts[16].position = { -1.0f, -1.0f, 1.0f };
		boxVerts[16].normal = { 0.0f, 0.0f, 1.0f };

		boxVerts[17].position = { 1.0f, 1.0f, 1.0f };
		boxVerts[17].normal = { 0.0f, 0.0f, 1.0f };

		boxVerts[18].position = { 1.0f, 1.0f, -1.0f };
		boxVerts[18].normal = { 0.0f, 1.0f, 0.0f };

		boxVerts[19].position = { 1.0f, 1.0f, -1.0f };
		boxVerts[19].normal = { 0.0f, 0.0f ,-1.0f };

		boxVerts[20].position = { 1.0f, 1.0f, 1.0f };
		boxVerts[20].normal = { 1.0f, 0.0f, 0.0f };

		boxVerts[21].position = { -1.0f, -1.0f, -1.0f };
		boxVerts[21].normal = { 0.0f ,-1.0f, 0.0f };

		boxVerts[22].position = { -1.0f, 1.0f, -1.0f };
		boxVerts[22].normal = { -1.0f, 0.0f, 0.0f };

		boxVerts[23].position = { -1.0f, 1.0f, 1.0f };
		boxVerts[23].normal = { 0.0f, 0.0f, 1.0f };

		bufferUploader.Upload((D12Resource*)vertexBuffer.GetAddressOf(), boxVerts, sizeof(Vertex) * G_VERTS, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		vertexBufferView.BufferLocation = vertexBuffer.Get()->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(Vertex);
		vertexBufferView.SizeInBytes = KBs(8);


		#define G_INDICES 36
		UINT32 indicies[G_INDICES];
		indicies[0] = 0;
		indicies[1] = 1;
		indicies[2] = 2;
		indicies[3] = 3;
		indicies[4] = 4;
		indicies[5] = 5;
		indicies[6] = 6;
		indicies[7] = 7;
		indicies[8] = 8;
		indicies[9] = 9;
		indicies[10] = 10;
		indicies[11] = 11;
		indicies[12] = 12;
		indicies[13] = 13;
		indicies[14] = 14;
		indicies[15] = 15;
		indicies[16] = 16;
		indicies[17] = 17;
		indicies[18] = 1;
		indicies[19] = 18;
		indicies[20] = 2;
		indicies[21] = 4;
		indicies[22] = 19;
		indicies[23] = 5;
		indicies[24] = 7;
		indicies[25] = 20;
		indicies[26] = 8;
		indicies[27] = 10;
		indicies[28] = 21;
		indicies[29] = 11;
		indicies[30] = 13;
		indicies[31] = 22;
		indicies[32] = 14;
		indicies[33] = 16;
		indicies[34] = 23;
		indicies[35] = 17;

		for (int i = 0; i < 18; i++)
		{
			int temp = indicies[i];
			indicies[i] = indicies[35 - i];
			indicies[35 - i] = temp;
		}

		indexBuffer.Initialize(device.Get(), KBs(16), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
		indexBuffer.Get()->SetName(L"Index buffer");
		
		bufferUploader.Upload((D12Resource*)indexBuffer.GetAddressOf(), indicies, sizeof(UINT32) * G_INDICES, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);

		indexBufferView.BufferLocation = indexBuffer.Get()->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		indexBufferView.SizeInBytes = KBs(16);

		
		basePipeline.Initialize(device.Get());
		depthBuffer.InitializeDepthBuffer(device.Get(), rWidth, rHeight);
		depthHeap.InitializeDepthHeap(device.Get());

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		device->CreateDepthStencilView(depthBuffer.Get(), &dsvDesc, depthHeap.Get()->GetCPUDescriptorHandleForHeapStart());

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

		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;

		viewMatrix = DirectX::XMMatrixLookAtLH({ 2.0f, 2.0f, -2.0f, 0.0f}, // camera pos
											   { 0.0f, 0.0f, 0.0f, 0.0f }, // looking at origin
											   { 0.0f, 1.0f, 0.0f, 0.0f });
		projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(1.5708f, 16.0f/9.0f, 1.0f, 50.0f); //fov 90deg, aspect, near, far
		viewProjMatrix = viewMatrix * projectionMatrix;

		PassDataBuffer.Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(PassData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
		PassDataBuffer.Get()->SetName(L"PassData buffer");

		Material material;
		material.albedo = { 0.8f, 0.0f, 0.0f, 1.0f };
		materialBuffer1.Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(Material)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
		materialBuffer1.Get()->SetName(L"Material buffer");
		bufferUploader.Upload((D12Resource*)materialBuffer1.GetAddressOf(), &material, sizeof(Material) * 1, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);
		

		lights[0].position = { 0.0f, 0.0f, 0.0f };
		lights[0].strength = 0.5f;
		lights[0].direction = { -0.5f, -1.0f, 0.0f };

	}

	void Renderer::UpdateDraw(){

		memcpy(PassDataBuffer.GetCPUMemory(), &viewProjMatrix, sizeof(PassData::viewprojmatrix));
		memcpy((BYTE*)PassDataBuffer.GetCPUMemory()+sizeof(PassData::viewprojmatrix), &lights[0], sizeof(Light));

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
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthHeap->GetCPUDescriptorHandleForHeapStart();
		
		cmdL.GraphicsCmd()->ClearRenderTargetView(rtvHandle, clearColor, 0 ,0);
		cmdL.GraphicsCmd()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0,0);
		cmdL.GraphicsCmd()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		cmdL.GraphicsCmd()->RSSetViewports(1, &viewport);
		cmdL.GraphicsCmd()->RSSetScissorRects(1, &SRRect);

		cmdL.GraphicsCmd()->SetGraphicsRootSignature(basePipeline.GetRootSignature());
		cmdL.GraphicsCmd()->SetPipelineState(basePipeline.Get());
		cmdL.GraphicsCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdL.GraphicsCmd()->IASetVertexBuffers(0, 1, &vertexBufferView);
		cmdL.GraphicsCmd()->IASetIndexBuffer(&indexBufferView);

		cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(0, PassDataBuffer.Get()->GetGPUVirtualAddress());
		cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(1, materialBuffer1.Get()->GetGPUVirtualAddress());

		//DRAW
		cmdL.GraphicsCmd()->DrawIndexedInstanced(G_INDICES, 1, 0, 0, 0);

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