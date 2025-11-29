#include "pch.h"
#include "Renderer.h"

#include <string>

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


		for (int i = 0; i < shadowTransforms.size(); i++)
		{
			shadowTransforms[i].Release();
		}
		for (int i = 0; i < objectTransforms.size(); i++)
		{
			objectTransforms[i].Release();
		}
		for (int i = 0; i < materialBuffers.size(); i++)
		{
			materialBuffers[i].Release();
		}
		PassDataBuffer.Release();
		depthHeap.Release();
		depthBuffer.Release();
		shadowPipeline.Release();
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

		std::vector<Vertex> vertices;
		std::vector<UINT32> indices;
		modelLoader.LoadFBXModel("Models/cubes.fbx", vertices, indices, meshes);

		vertexBuffer.Initialize(device.Get(), KBs(8), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
		vertexBuffer.Get()->SetName(L"Vertex buffer");

		bufferUploader.Upload((D12Resource*)vertexBuffer.GetAddressOf(), vertices.data(), sizeof(Vertex) * vertices.size(), (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		vertexBufferView.BufferLocation = vertexBuffer.Get()->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(Vertex);
		vertexBufferView.SizeInBytes = KBs(8);

		indexBuffer.Initialize(device.Get(), KBs(16), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
		indexBuffer.Get()->SetName(L"Index buffer");
		
		bufferUploader.Upload((D12Resource*)indexBuffer.GetAddressOf(), indices.data(), sizeof(UINT32) * indices.size(), (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);

		indexBufferView.BufferLocation = indexBuffer.Get()->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		indexBufferView.SizeInBytes = KBs(16);

		
		basePipeline.Initialize(device.Get());
		shadowPipeline.InitializeAsTransparent(device.Get());
		depthBuffer.InitializeDepthBuffer(device.Get(), rWidth, rHeight);
		depthHeap.InitializeDepthHeap(device.Get());

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
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

		viewMatrix = DirectX::XMMatrixLookAtLH({ -3.0f, 10.0f, -10.0f, 0.0f}, // camera pos
											   { 0.0f, 0.0f, 0.0f, 0.0f }, // looking at origin
											   { 0.0f, 1.0f, 0.0f, 0.0f });
		viewProjMatrix = viewMatrix * projectionMatrix;

		PassDataBuffer.Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(PassData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
		PassDataBuffer.Get()->SetName(L"PassData buffer");


		{
			materialBuffers.emplace_back(D12Resource());
			Material material1;
			material1.albedo = { 0.5f, 0.0f, 0.0f, 1.0f };
			materialBuffers[0].Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(Material)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			materialBuffers[0]->SetName(L"Material buffer 0");
			bufferUploader.Upload((D12Resource*)materialBuffers[0].GetAddressOf(), &material1, sizeof(Material) * 1, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);

			materialBuffers.emplace_back(D12Resource());
			Material material2;
			material2.albedo = { 0.0f, 0.5f, 0.0f, 1.0f };
			materialBuffers[1].Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(Material)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			materialBuffers[1]->SetName(L"Material buffer 1");
			bufferUploader.Upload((D12Resource*)materialBuffers[1].GetAddressOf(), &material2, sizeof(Material) * 1, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);
		
			materialBuffers.emplace_back(D12Resource());
			Material material3;
			material3.albedo = { 0.2f, 0.2f, 0.2f, 1.0f };
			materialBuffers[2].Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(Material)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			materialBuffers[2]->SetName(L"Material buffer 2");
			bufferUploader.Upload((D12Resource*)materialBuffers[2].GetAddressOf(), &material3, sizeof(Material) * 1, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);
		
			materialBuffers.emplace_back(D12Resource());
			Material material_shadow;
			material_shadow.albedo = { 0.0f, 0.0f, 0.0f, 0.5f };
			materialBuffers[3].Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(Material)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			materialBuffers[3]->SetName(L"Material buffer shadow");
			bufferUploader.Upload((D12Resource*)materialBuffers[3].GetAddressOf(), &material_shadow, sizeof(Material) * 1, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);
		
		}

		lights[0].position = { 0.0f,0.0f,0.0f };
		lights[0].strength = 1.0f;
		lights[0].direction = { 1.0f,-1.0f,0.0f };

		{
			ObjectData tempData;
			tempData.transform.r[0] = { 1.0f, 0.0f, 0.0f, 0.0f }; // x scale
			tempData.transform.r[1] = { 0.0f, 2.0f, 0.0f, 0.0f }; // y scale
			tempData.transform.r[2] = { 0.0f, 0.0f, 5.0f, 0.0f }; // z scale
			tempData.transform.r[3] = { -5.0f, 5.0f, -2.0f, 1.0f }; // xyz pos
			objectTransformsCPU.push_back(tempData);
			objectTransforms.emplace_back(D12Resource());
			objectTransforms[0].Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			objectTransforms[0]->SetName(L"Object transform 0");
			memcpy(objectTransforms[0].GetCPUMemory(), &tempData, sizeof(ObjectData));

			tempData.transform = DirectX::XMMatrixIdentity();
			tempData.transform.r[0] = { 2.0f, 0.0f, 0.0f, 0.0f }; // x scale
			tempData.transform.r[1] = { 0.0f, 1.0f, 0.0f, 0.0f }; // y scale
			tempData.transform.r[2] = { 0.0f, 0.0f, 2.0f, 0.0f }; // z scale
			tempData.transform.r[3] = { 2.0f, 5.0f, 5.0f, 1.0f }; // xyz pos
			objectTransformsCPU.push_back(tempData);
			objectTransforms.emplace_back(D12Resource());
			objectTransforms[1].Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			objectTransforms[1]->SetName(L"Object transform 1");
			memcpy(objectTransforms[1].GetCPUMemory(), &tempData, sizeof(ObjectData));

			tempData.transform = DirectX::XMMatrixIdentity();
			tempData.transform.r[0] = { 1000.0f, 0.0f,  0.0f,    0.0f };
			tempData.transform.r[1] = { 0.0f,    1.0f,  0.0f,    0.0f };
			tempData.transform.r[2] = { 0.0f,    0.0,   1000.0f, 0.0f };
			tempData.transform.r[3] = { 0.0f,   -1.0f,  0.0f,    1.0f };

			objectTransforms.emplace_back(D12Resource());
			objectTransforms[2].Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			objectTransforms[2]->SetName(L"Object transform 2 (floor)");
			objectTransformsCPU.push_back(tempData);

			memcpy(objectTransforms[2].GetCPUMemory(), &tempData, sizeof(ObjectData));

			tempData.transform = DirectX::XMMatrixIdentity();
			tempData.transform.r[0] = { 1.0f, 0.0f, 0.0f, 0.0f }; // x scale
			tempData.transform.r[1] = { 0.0f, 1.0f, 0.0f, 0.0f }; // y scale
			tempData.transform.r[2] = { 0.0f, 0.0f, 1.0f, 0.0f }; // z scale
			tempData.transform.r[3] = { 3.0f, 5.0f, 0.0f, 1.0f }; // xyz pos
			objectTransformsCPU.push_back(tempData);
			objectTransforms.emplace_back(D12Resource());
			objectTransforms[3].Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			objectTransforms[3]->SetName(L"Object transform 3");
			memcpy(objectTransforms[3].GetCPUMemory(), &tempData, sizeof(ObjectData));
		}

		//Shadow transforms
		{
			shadowTransforms.resize(objectTransforms.size());


			for (int i = 0; i < objectTransforms.size(); i++) {
				shadowTransforms[i].Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
				std::wstring name = L"Shadow transform buffer ";
				name.append(std::to_wstring(i));

				shadowTransforms[i]->SetName(name.c_str());

				ObjectData tempData;

				memcpy(shadowTransforms[i].GetCPUMemory(), &tempData, sizeof(ObjectData));


			}
		}
	}

	void Renderer::UpdateDraw(const float dt){

		viewMatrix = DirectX::XMMatrixLookAtLH(	{ -3.0f, 10.0f, -10.0f, 0.0f }, // camera pos
												{ 0.0f, 0.0f, 0.0f, 0.0f }, // looking at origin
												{ 0.0f, 1.0f, 0.0f, 0.0f });
		viewProjMatrix = viewMatrix * projectionMatrix;

		{

			memcpy(PassDataBuffer.GetCPUMemory(), &viewProjMatrix, sizeof(PassData::viewprojmatrix));
			memcpy((BYTE*)PassDataBuffer.GetCPUMemory()+sizeof(PassData::viewprojmatrix), &lights[0], sizeof(Light));

			DirectX::XMVECTOR planeToCastShadow = { 0.0f,1.0f,0.0f,0.0f };
			DirectX::XMVECTOR dirToLightSource = DirectX::XMVectorNegate(DirectX::XMLoadFloat3(&lights[0].direction));

			DirectX::XMMATRIX shadowMatrix = DirectX::XMMatrixShadow(planeToCastShadow, dirToLightSource);
			DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.001f, 0.0f);


			for (int i = 0; i < shadowTransforms.size(); i++) {
				ObjectData tempData;
				tempData.transform = objectTransformsCPU[i].transform * shadowMatrix * translation;
				memcpy(shadowTransforms[i].GetCPUMemory(), &tempData, sizeof(ObjectData));

			}

		}

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
		cmdL.GraphicsCmd()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0,0);
		cmdL.GraphicsCmd()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		cmdL.GraphicsCmd()->RSSetViewports(1, &viewport);
		cmdL.GraphicsCmd()->RSSetScissorRects(1, &SRRect);

		cmdL.GraphicsCmd()->SetGraphicsRootSignature(basePipeline.GetRootSignature());

		cmdL.GraphicsCmd()->SetPipelineState(basePipeline.Get());
		cmdL.GraphicsCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdL.GraphicsCmd()->IASetVertexBuffers(0, 1, &vertexBufferView);
		cmdL.GraphicsCmd()->IASetIndexBuffer(&indexBufferView);
		cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(0, PassDataBuffer.Get()->GetGPUVirtualAddress());

		{
			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(1, objectTransforms[0]->GetGPUVirtualAddress());
			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(2, materialBuffers[0].Get()->GetGPUVirtualAddress());

			Render::MeshDataRAW* rawMeshData = &meshes[1];
			cmdL.GraphicsCmd()->DrawIndexedInstanced(rawMeshData->indexCount, 1, rawMeshData->indexOffset, rawMeshData->vertexOffset, 0);

		}
		{
			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(1, objectTransforms[1]->GetGPUVirtualAddress());
			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(2, materialBuffers[1].Get()->GetGPUVirtualAddress());

			Render::MeshDataRAW* rawMeshData = &meshes[2];
			cmdL.GraphicsCmd()->DrawIndexedInstanced(rawMeshData->indexCount, 1, rawMeshData->indexOffset, rawMeshData->vertexOffset, 0);

		}
		{
			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(1, objectTransforms[2]->GetGPUVirtualAddress());
			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(2, materialBuffers[2].Get()->GetGPUVirtualAddress());

			Render::MeshDataRAW* rawMeshData = &meshes[0];
			cmdL.GraphicsCmd()->DrawIndexedInstanced(rawMeshData->indexCount, 1, rawMeshData->indexOffset, rawMeshData->vertexOffset, 0);

		}

		cmdL.GraphicsCmd()->SetPipelineState(shadowPipeline.Get());
		cmdL.GraphicsCmd()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		{

			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(1, shadowTransforms[0].Get()->GetGPUVirtualAddress());
			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(2, materialBuffers[3].Get()->GetGPUVirtualAddress());

			Render::MeshDataRAW* rawMeshData = &meshes[1];
			cmdL.GraphicsCmd()->DrawIndexedInstanced(rawMeshData->indexCount, 1, rawMeshData->indexOffset, rawMeshData->vertexOffset, 0);

		}
		{

			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(1, shadowTransforms[1].Get()->GetGPUVirtualAddress());
			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(2, materialBuffers[3].Get()->GetGPUVirtualAddress());

			Render::MeshDataRAW* rawMeshData = &meshes[2];
			cmdL.GraphicsCmd()->DrawIndexedInstanced(rawMeshData->indexCount, 1, rawMeshData->indexOffset, rawMeshData->vertexOffset, 0);

		}

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