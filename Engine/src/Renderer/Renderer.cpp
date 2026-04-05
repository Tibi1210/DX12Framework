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

		for (auto& obj : scene.elements) {
			obj.get()->Release();
		}
		PassDataBuffer.Release();

		depthHeap.Release();
		depthBuffers[0].Release();
		depthBuffers[1].Release();

		shadowMapPipeline.Release();

		for (int i = 0; i < 3; i++){
			defferedOutputTextures[i].Release();
		}
		SRVHeap.Release();
		defferedRenderTargetHeap.Release();
		defferedPixelPipeline.Release();
		defferedPipeline.Release();

		for (int i = 0; i < 2; i++) {
			transformResource[i].Release();
			materialResource[i].Release();
			indexBuffer[i].Release();
			vertexBuffer[i].Release();
		}

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
		constexpr unsigned int shadowmapRes = 2048;

		// disable on prod
		D12Debug::Get().Enable();
		DXGIDebug::Get().Enable();

		DXGIFactory factory;
		DXGIAdapter adapter = factory.GetAdapter();
		
		// ESSENTIALS
		{	
			device.Initialize(adapter.Get());
			cmdQ.Initialize(device.Get());
			cmdL.Initialize(device.Get());
			swapchain.Initialize(device.Get(), factory.Get(), cmdQ.Get(), hwnd, rWidth, rHeight);
			bufferUploader.Initialize(device.Get(), KBs(1024));
		}
		
		// MODEL LOADING
		{	

			scene.elements.clear();  // Clear scene

			// FBX 1 floor
			{

				scene.elements.emplace_back(std::make_unique<Mesh>());
				Mesh* currentMesh = static_cast<Mesh*>(scene.elements.back().get());
				currentMesh->name = "Floor";
				currentMesh->Load("Models/floor.fbx");
				DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(100.0f, 0.1f, 100.0f);
				DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(0.0f)) *
											 DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(0.0f)) *
											 DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(0.0f));
				DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, -5.0f, 0.0f);
				DirectX::XMMATRIX transformAll = scale * rotation * translation;
				for (int i = 0; i < currentMesh->geometries.size(); i++){
					MeshDataRAW& geom = currentMesh->geometries[i];
					geom.includeInShadowMap = false;
					ObjectData& data = currentMesh->datas[i];
					data.transform = transformAll;
					Material& material = currentMesh->materials[i];
					material.albedo = { 0.4f, 0.4f, 0.4f, 1.0f };
				}

				PRINT_N("VERTEX BUFFER SIZE: " << currentMesh->vertexBufferSize);
				PRINT_N("INDEX BUFFER SIZE: " << currentMesh->indexBufferSize);

				vertexBuffer[0].Initialize(device.Get(), currentMesh->vertexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
				vertexBuffer[0].Get()->SetName(L"Vertex buffer Floor");

				bufferUploader.Upload((D12Resource*)vertexBuffer[0].GetAddressOf(), currentMesh->vertices.data(), currentMesh->vertexBufferSize, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

				vertexBufferView[0].BufferLocation = vertexBuffer[0].Get()->GetGPUVirtualAddress();
				vertexBufferView[0].StrideInBytes = sizeof(Vertex);
				vertexBufferView[0].SizeInBytes = currentMesh->vertexBufferSize;

				indexBuffer[0].Initialize(device.Get(), currentMesh->indexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
				indexBuffer[0].Get()->SetName(L"Index buffer Floor");

				bufferUploader.Upload((D12Resource*)indexBuffer[0].GetAddressOf(), currentMesh->indices.data(), currentMesh->indexBufferSize, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);

				indexBufferView[0].BufferLocation = indexBuffer[0].Get()->GetGPUVirtualAddress();
				indexBufferView[0].Format = DXGI_FORMAT_R32_UINT;
				indexBufferView[0].SizeInBytes = currentMesh->indexBufferSize;

			}

			// FBX 2 goober
			{
				scene.elements.emplace_back(std::make_unique<Mesh>());
				Mesh* currentMesh = static_cast<Mesh*>(scene.elements.back().get());
				currentMesh->name = "Glorp";
				currentMesh->Load("Models/glorp.fbx");

				DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(2.0f, 2.0f, 2.0f);
				DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(-90.0f)) *
											 DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(0.0f)) *
											 DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(0.0f));
				DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
				DirectX::XMMATRIX transformAll = scale * rotation * translation;

				for (int i = 0; i < currentMesh->geometries.size(); i++) {
					MeshDataRAW& geom = currentMesh->geometries[i];
					geom.includeInShadowMap = true;
					ObjectData& data = currentMesh->datas[i];
					data.transform = transformAll;
					Material& material = currentMesh->materials[i];
					material.albedo = { 0.0f,((float)i + 1) / 10, 0.0f, 1.0f };
				}
				PRINT_N("VERTEX BUFFER SIZE: " << currentMesh->vertexBufferSize);
				PRINT_N("INDEX BUFFER SIZE: " << currentMesh->indexBufferSize);
				vertexBuffer[1].Initialize(device.Get(), currentMesh->vertexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
				vertexBuffer[1].Get()->SetName(L"Vertex buffer Glorp");

				bufferUploader.Upload((D12Resource*)vertexBuffer[1].GetAddressOf(), currentMesh->vertices.data(), currentMesh->vertexBufferSize, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

				vertexBufferView[1].BufferLocation = vertexBuffer[1].Get()->GetGPUVirtualAddress();
				vertexBufferView[1].StrideInBytes = sizeof(Vertex);
				vertexBufferView[1].SizeInBytes = currentMesh->vertexBufferSize;

				indexBuffer[1].Initialize(device.Get(), currentMesh->indexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
				indexBuffer[1].Get()->SetName(L"Index buffer Glorp");

				bufferUploader.Upload((D12Resource*)indexBuffer[1].GetAddressOf(), currentMesh->indices.data(), currentMesh->indexBufferSize, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);

				indexBufferView[1].BufferLocation = indexBuffer[1].Get()->GetGPUVirtualAddress();
				indexBufferView[1].Format = DXGI_FORMAT_R32_UINT;
				indexBufferView[1].SizeInBytes = currentMesh->indexBufferSize;
			}


		}

		// DESCRIPTOR HEAPS
		const unsigned int renderTargets = 3;
		{

			defferedRenderTargetHeap.InitializeRTV(device.Get(), renderTargets);
			defferedRenderTargetHeap->SetName(L"Deffered descriptor heap");

			SRVHeap.InitializeCBVSRVUAV(device.Get(), 5);
			SRVHeap->SetName(L"Main CBV, SRV, UAV descriptor heap");

			depthHeap.InitializeDepthHeap(device.Get(), 2);
			depthHeap->SetName(L"Depth descriptor heap");
		}

		// deffered pipeline
		{

			defferedPipeline.InitializeDeffered(device.Get());
			defferedPipeline->SetName(L"Deffered pipeline");

			defferedPixelPipeline.InitializeDefferedPixel(device.Get(), L"Shaders/postfx/lights/pixel.hlsl");
			defferedPixelPipeline->SetName(L"Deffered Pixel pipeline");

			for (int i = 0; i < renderTargets; i++) {

				defferedOutputTextures[i].InitializeTexture(device.Get(), rWidth, rHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);

				CD3DX12_RESOURCE_BARRIER transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
					defferedOutputTextures[i].Get(),
					D3D12_RESOURCE_STATE_COMMON,
					D3D12_RESOURCE_STATE_RENDER_TARGET);
				cmdL.GraphicsCmd()->ResourceBarrier(1, &transitionBarrier);

				device->CreateRenderTargetView(defferedOutputTextures[i].Get(), nullptr, defferedRenderTargetHeap.GetCPUHandle(i));
				device->CreateShaderResourceView(defferedOutputTextures[i].Get(), &defferedOutputTextures[i].GetSRV(), SRVHeap.GetCPUHandle(i));

				std::wstring name = L"G-BUFFER render target #";
				name.append(std::to_wstring(i + 1));
				defferedOutputTextures[i]->SetName(name.c_str());
			}
		}

		// shadowmap pipeline
		{
			shadowMapPipeline.InitializeShadowMap(device.Get());
			shadowMapPipeline->SetName(L"ShadowMap pipeline");
		}

		// DEPTH
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC shaderResView = {};
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			// DEPTHBUFFER
			{
				depthBuffers[0].InitializeDepthBuffer(device.Get(), rWidth, rHeight);
				depthBuffers[0]->SetName(L"G-BUFFER Depth buffer");

				dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;
				dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

				shaderResView.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				shaderResView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				shaderResView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				shaderResView.Texture2D.MipLevels = 1;
				shaderResView.Texture2D.MostDetailedMip = 0;
				shaderResView.Texture2D.PlaneSlice = 0;
				shaderResView.Texture2D.ResourceMinLODClamp = 0.0f;

				device->CreateDepthStencilView(depthBuffers[0].Get(), &dsvDesc, depthHeap.GetCPUHandle(0));
				device->CreateShaderResourceView(depthBuffers[0].Get(), &shaderResView, SRVHeap.GetCPUHandle(3));
			}

			// SHADOWMAP
			depthBuffers[1].InitializeDepthBuffer(device.Get(), shadowmapRes, shadowmapRes, DXGI_FORMAT_D32_FLOAT);
			depthBuffers[1]->SetName(L"ShadowMap Depth buffer");

			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

			shaderResView.Format = DXGI_FORMAT_R32_FLOAT;
			shaderResView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			shaderResView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			shaderResView.Texture2D.MipLevels = 1;
			shaderResView.Texture2D.MostDetailedMip = 0;
			shaderResView.Texture2D.PlaneSlice = 0;
			shaderResView.Texture2D.ResourceMinLODClamp = 0.0f;

			device->CreateDepthStencilView(depthBuffers[1].Get(), &dsvDesc, depthHeap.GetCPUHandle(1));
			device->CreateShaderResourceView(depthBuffers[1].Get(), &shaderResView, SRVHeap.GetCPUHandle(4));
		}

		// viewport and scissor rect
		{
			// regular
			viewport[0].TopLeftX = 0;
			viewport[0].TopLeftY = 0;
			viewport[0].Width = rWidth;
			viewport[0].Height = rHeight;
			viewport[0].MinDepth = 0.0f;
			viewport[0].MaxDepth = 1.0f;

			SRRect[0].left = 0;
			SRRect[0].right = viewport[0].Width;
			SRRect[0].top = 0;
			SRRect[0].bottom = viewport[0].Height;

			// shadowmap
			viewport[1].TopLeftX = 0;
			viewport[1].TopLeftY = 0;
			viewport[1].Width = shadowmapRes;
			viewport[1].Height = shadowmapRes;
			viewport[1].MinDepth = 0.0f;
			viewport[1].MaxDepth = 1.0f;

			SRRect[1].left = 0;
			SRRect[1].right = viewport[1].Width;
			SRRect[1].top = 0;
			SRRect[1].bottom = viewport[1].Height;
		}

		// PassData init
		{
			viewMatrix = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 10.0f, 0.0f }, // camera pos
												   { 0.0f, 0.0f, 0.0f, 0.0f }, // looking at origin
												   { 0.0f, 1.0f, 0.0f, 0.0f });
			viewProjMatrix = viewMatrix * projectionMatrix;

			PassDataBuffer.Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(PassData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			PassDataBuffer.Get()->SetName(L"PassData buffer");
		}

		// materials
		{
			{
				Mesh* currentMesh = static_cast<Mesh*>(scene.elements[0].get());
				UINT materialCount = currentMesh->materials.size();
				UINT byteSize = materialCount * sizeof(Material);
				
				materialResource[0].Initialize(device.Get(), Utils::CalcConstBufferAlignment(byteSize), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
				materialResource[0].Get()->SetName(L"Material buffer for Floor");
				//bufferUploader.Upload((D12Resource*)materialResource[0].GetAddressOf(), currentMesh->materials.data(), byteSize, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);
				memcpy(materialResource[0].GetCPUMemory(), currentMesh->materials.data(), byteSize);
			}
			{
				Mesh* currentMesh = static_cast<Mesh*>(scene.elements[1].get());
				UINT materialCount = currentMesh->materials.size();
				UINT byteSize = materialCount * sizeof(Material);

				materialResource[1].Initialize(device.Get(), Utils::CalcConstBufferAlignment(byteSize), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
				materialResource[1].Get()->SetName(L"Material buffer for Glorp");
				//bufferUploader.Upload((D12Resource*)materialResource[1].GetAddressOf(), currentMesh->materials.data(), byteSize, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);
				memcpy(materialResource[1].GetCPUMemory(), currentMesh->materials.data(), byteSize);
			}
		}
		// obj transforms
		{
			{
				Mesh* currentMesh = static_cast<Mesh*>(scene.elements[0].get());
				UINT dataCount = currentMesh->datas.size();
				UINT byteSize = dataCount * sizeof(ObjectData);

				transformResource[0].Initialize(device.Get(), Utils::CalcConstBufferAlignment(byteSize), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
				transformResource[0].Get()->SetName(L"Transform buffer for Floor");
				memcpy(transformResource[0].GetCPUMemory(), currentMesh->datas.data(), byteSize);
			}
			{
				Mesh* currentMesh = static_cast<Mesh*>(scene.elements[1].get());
				UINT dataCount = currentMesh->datas.size();
				UINT byteSize = dataCount * sizeof(ObjectData);

				transformResource[1].Initialize(device.Get(), Utils::CalcConstBufferAlignment(byteSize), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
				transformResource[1].Get()->SetName(L"Transform buffer for Glorp");
				memcpy(transformResource[1].GetCPUMemory(), currentMesh->datas.data(), byteSize);
			}
		}

		lights[0].position = { 0.0f,0.0f,0.0f };
		lights[0].strength = 1.0f;
		lights[0].direction = { 1.0f,-1.0f,0.0f };

	}

	void Renderer::Update(const float dt, const float gameTime)
	{
		PassData passData;

		float shadowmapRadius = 15.0f;
		DirectX::XMVECTOR sceneCenter = { 0.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR lightPos = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&lights[0].direction), -shadowmapRadius);
		DirectX::XMMATRIX viewLights = DirectX::XMMatrixLookAtLH(lightPos, sceneCenter, { 0.0f, 1.0f, 0.0f, 0.0f });
		DirectX::XMMATRIX lightProjMatrix = DirectX::XMMatrixOrthographicLH(20, 20, 0.5f, 200.0f);

		switch (inputHandler.keyValue) {
			case 65: {
				asd += 10.0f * dt;
				break;
			}
			case 68: {
				asd -= 10.0f * dt;
				break;
			}
			case 87: {
				asd1 -= 10.0f * dt;
				break;
			}
			case 83: {
				asd1 += 10.0f * dt;
				break;
			}
			case 17: {
				asd2 -= 10.0f * dt;
				break;
			}
			case 32: {
				asd2 += 10.0f * dt;
				break;
			}

		}
		viewMatrix = DirectX::XMMatrixLookAtLH({ asd, asd2, asd1, 0.0f }, // camera pos
											   { 0.0f, 0.0f, 0.0f, 0.0f }, // looking at origin
											   { 0.0f, 1.0f, 0.0f, 0.0f });

		viewProjMatrix = viewMatrix * projectionMatrix;
		passData.viewprojmatrix = viewProjMatrix;

		passData.sceneLight = lights[0];
		passData.lightviewprojmatrix = viewLights * lightProjMatrix;

		memcpy(PassDataBuffer.GetCPUMemory(), &passData, sizeof(PassData));

	}
	void Renderer::Draw()
	{
		ID3D12DescriptorHeap* descHeaps[1] = { SRVHeap.Get() };
		cmdL.GraphicsCmd()->SetDescriptorHeaps(1, descHeaps);

		// draw to shadowmap
		{
			cmdL.GraphicsCmd()->RSSetViewports(1, &viewport[1]);
			cmdL.GraphicsCmd()->RSSetScissorRects(1, &SRRect[1]);

			cmdL.GraphicsCmd()->SetGraphicsRootSignature(shadowMapPipeline.GetRootSignature());
			cmdL.GraphicsCmd()->SetPipelineState(shadowMapPipeline.Get());
			cmdL.GraphicsCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// clear depthbuffer for shadowmap
			{
				D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthHeap.GetCPUHandle(1); // shadowmap depth buffer
				cmdL.GraphicsCmd()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, 0);
				cmdL.GraphicsCmd()->OMSetRenderTargets(0, nullptr, false, &dsvHandle);
			}
			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(0, PassDataBuffer.Get()->GetGPUVirtualAddress());

			// draw call
			cmdL.GraphicsCmd()->IASetVertexBuffers(0, 1, &vertexBufferView[1]);
			cmdL.GraphicsCmd()->IASetIndexBuffer(&indexBufferView[1]);
			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(1, transformResource[1].Get()->GetGPUVirtualAddress());
			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(2, materialResource[1].Get()->GetGPUVirtualAddress());

			Mesh* currentMesh = static_cast<Mesh*>(scene.elements[1].get());
			for (MeshDataRAW& geom : currentMesh->geometries) {
				if (geom.includeInShadowMap) {
					cmdL.GraphicsCmd()->DrawIndexedInstanced(geom.indexCount, 1, geom.indexOffset, geom.vertexOffset, 0);
				}
			}

		}

		// draw to frame
		{
			cmdL.GraphicsCmd()->RSSetViewports(1, &viewport[0]);
			cmdL.GraphicsCmd()->RSSetScissorRects(1, &SRRect[0]);

			cmdL.GraphicsCmd()->SetGraphicsRootSignature(defferedPipeline.GetRootSignature());
			cmdL.GraphicsCmd()->SetPipelineState(defferedPipeline.Get());
			cmdL.GraphicsCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// clear
			{
				const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
				D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[3] = { defferedRenderTargetHeap.GetCPUHandle(0),
															 defferedRenderTargetHeap.GetCPUHandle(1),
															 defferedRenderTargetHeap.GetCPUHandle(2) };
				D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthHeap.GetCPUHandle(0);
				cmdL.GraphicsCmd()->ClearRenderTargetView(rtvHandle[0], clearColor, 0, 0);
				cmdL.GraphicsCmd()->ClearRenderTargetView(rtvHandle[1], clearColor, 0, 0);
				cmdL.GraphicsCmd()->ClearRenderTargetView(rtvHandle[2], clearColor, 0, 0);
				cmdL.GraphicsCmd()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, 0);
				cmdL.GraphicsCmd()->OMSetRenderTargets(3, rtvHandle, false, &dsvHandle);
			}
			cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(0, PassDataBuffer.Get()->GetGPUVirtualAddress());

			// draw call
			{
				cmdL.GraphicsCmd()->IASetVertexBuffers(0, 1, &vertexBufferView[0]);
				cmdL.GraphicsCmd()->IASetIndexBuffer(&indexBufferView[0]);
				cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(1, transformResource[0].Get()->GetGPUVirtualAddress());
				cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(2, materialResource[0].Get()->GetGPUVirtualAddress());

				Mesh* currentMesh = static_cast<Mesh*>(scene.elements[0].get());
				for (MeshDataRAW& geom : currentMesh->geometries) {
					cmdL.GraphicsCmd()->DrawIndexedInstanced(geom.indexCount, 1, geom.indexOffset, geom.vertexOffset, 0);
				}
			}
			{
				cmdL.GraphicsCmd()->IASetVertexBuffers(0, 1, &vertexBufferView[1]);
				cmdL.GraphicsCmd()->IASetIndexBuffer(&indexBufferView[1]);
				cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(1, transformResource[1].Get()->GetGPUVirtualAddress());
				cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(2, materialResource[1].Get()->GetGPUVirtualAddress());

				Mesh* currentMesh = static_cast<Mesh*>(scene.elements[1].get());
				for (MeshDataRAW& geom : currentMesh->geometries) {
					cmdL.GraphicsCmd()->DrawIndexedInstanced(geom.indexCount, 1, geom.indexOffset, geom.vertexOffset, 0);
				}
			}

		}

		// set swapchain back buffer to render target stage
		{
			CD3DX12_RESOURCE_BARRIER transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
				swapchain.GetCurrentRT(),
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET);
			cmdL.GraphicsCmd()->ResourceBarrier(1, &transitionBarrier);
		}

		cmdL.GraphicsCmd()->SetGraphicsRootSignature(defferedPixelPipeline.GetRootSignature());
		cmdL.GraphicsCmd()->SetPipelineState(defferedPixelPipeline.Get());
		cmdL.GraphicsCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdL.GraphicsCmd()->IASetVertexBuffers(0, 1, &vertexBufferView[0]);
		cmdL.GraphicsCmd()->IASetIndexBuffer(&indexBufferView[0]);

		// clear swapchain and depthbuffer bind as output
		{
			const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapchain.GetCurrentRTVHandle();
			cmdL.GraphicsCmd()->ClearRenderTargetView(rtvHandle, clearColor, 0, 0);
			cmdL.GraphicsCmd()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
		}

		cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(0, PassDataBuffer.Get()->GetGPUVirtualAddress());
		cmdL.GraphicsCmd()->SetGraphicsRootDescriptorTable(3, SRVHeap.GetGPUHandle(0));

		cmdL.GraphicsCmd()->DrawIndexedInstanced(3, 1, 0, 0, 0);

		// set swapchain back buffer to present stage
		{
			CD3DX12_RESOURCE_BARRIER transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
				swapchain.GetCurrentRT(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT);
			cmdL.GraphicsCmd()->ResourceBarrier(1, &transitionBarrier);
		}

		// command list close/execute/present
		{
			cmdL.GraphicsCmd()->Close();
			cmdQ.ExecuteCmdList(cmdL.Get());

			swapchain.Present();
		}

		if (cmdQ.GetFence()->GetCompletedValue() < cmdQ.M_GetCurrentFence())
		{
			HANDLE eventHandle = CreateEventEx(nullptr, L"FenceCompletedEventHandler", false, EVENT_ALL_ACCESS);
			EVAL_HRES(cmdQ.GetFence()->SetEventOnCompletion(cmdQ.M_GetCurrentFence(), eventHandle), "Error completing fence.");
			if (eventHandle != nullptr)
			{
				WaitForSingleObject(eventHandle, INFINITE);
				CloseHandle(eventHandle);
			}
		}

		cmdL.ResetCmd();
	}

	void Renderer::handleInput(const int event, const int x, const int y){
		inputHandler.handleInput(event, x, y);
		//PRINT_N(inputHandler.keyValue);
	}
}