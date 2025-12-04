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
			bufferUploader.Initialize(device.Get(), KBs(1400));
		}
		
		// MODEL LOADING
		{	

			scene.elements.clear();  // Clear scene

			// FBX 1 floor
			{
				size_t vertexBufferSize = 0;
				size_t indexBufferSize = 0;
				std::vector<Vertex> vertices;
				std::vector<UINT32> indices;

				scene.elements.emplace_back(std::make_unique<Object>());
				static_cast<Object*>(scene.elements[0].get())->name = "Object 1";
				static_cast<Mesh*>(scene.elements[0].get())->Load("Models/floor.fbx");

				DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(100.0f, 0.1f, 100.0f);
				DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(0.0f)) *
											 DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(0.0f)) *
											 DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(0.0f));
				DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
				DirectX::XMMATRIX transformAll = scale * rotation * translation;

				for (auto& mesh : scene.elements[0].get()->elements) {
					if (dynamic_cast<Mesh*>(mesh.get()) != nullptr) {
						static_cast<Mesh*>(mesh.get())->includeInShadowMap = false;
						static_cast<Mesh*>(mesh.get())->transform.transform = transformAll;
						static_cast<Mesh*>(mesh.get())->material.albedo = { 0.4f, 0.4f, 0.4f, 1.0f };
						vertexBufferSize += static_cast<Mesh*>(mesh.get())->mesh.vertexCount * sizeof(Vertex);
						indexBufferSize += static_cast<Mesh*>(mesh.get())->mesh.indexCount * sizeof(UINT32);
						vertices.insert(vertices.end(), static_cast<Mesh*>(mesh.get())->vertices.begin(), static_cast<Mesh*>(mesh.get())->vertices.end());
						indices.insert(indices.end(), static_cast<Mesh*>(mesh.get())->indices.begin(), static_cast<Mesh*>(mesh.get())->indices.end());
					}
				}
				PRINT_N("vertex size: " << vertexBufferSize);
				PRINT_N("index size: " << indexBufferSize);

				vertexBuffer[0].Initialize(device.Get(), vertexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
				vertexBuffer[0].Get()->SetName(L"Vertex buffer Object 1");

				bufferUploader.Upload((D12Resource*)vertexBuffer[0].GetAddressOf(), vertices.data(), sizeof(Vertex) * vertices.size(), (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

				vertexBufferView[0].BufferLocation = vertexBuffer[0].Get()->GetGPUVirtualAddress();
				vertexBufferView[0].StrideInBytes = sizeof(Vertex);
				vertexBufferView[0].SizeInBytes = vertexBufferSize;

				indexBuffer[0].Initialize(device.Get(), indexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
				indexBuffer[0].Get()->SetName(L"Index buffer Object 1");

				bufferUploader.Upload((D12Resource*)indexBuffer[0].GetAddressOf(), indices.data(), sizeof(UINT32) * indices.size(), (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);

				indexBufferView[0].BufferLocation = indexBuffer[0].Get()->GetGPUVirtualAddress();
				indexBufferView[0].Format = DXGI_FORMAT_R32_UINT;
				indexBufferView[0].SizeInBytes = indexBufferSize;

			}

			// FBX 2 goober
			{
				size_t vertexBufferSize = 0;
				size_t indexBufferSize = 0;
				std::vector<Vertex> vertices;
				std::vector<UINT32> indices;
				scene.elements.emplace_back(std::make_unique<Object>());
				static_cast<Object*>(scene.elements[1].get())->name = "Object 2";
				static_cast<Mesh*>(scene.elements[1].get())->Load("Models/glorp.fbx");

				DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(2.0f, 2.0f, 2.0f);
				DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(-90.0f)) *
											 DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(0.0f)) *
											 DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(0.0f));
				DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 5.0f, 0.0f);
				DirectX::XMMATRIX transformAll = scale * rotation * translation;


				for (auto& mesh : scene.elements[1].get()->elements) {
					if (dynamic_cast<Mesh*>(mesh.get()) != nullptr) {
						static_cast<Mesh*>(mesh.get())->includeInShadowMap = true;
						static_cast<Mesh*>(mesh.get())->transform.transform = transformAll;
						static_cast<Mesh*>(mesh.get())->material.albedo = { 0.0f, 0.4f, 0.0f, 1.0f };
						vertexBufferSize += static_cast<Mesh*>(mesh.get())->mesh.vertexCount * sizeof(Vertex);
						indexBufferSize += static_cast<Mesh*>(mesh.get())->mesh.indexCount * sizeof(UINT32);
						vertices.insert(vertices.end(), static_cast<Mesh*>(mesh.get())->vertices.begin(), static_cast<Mesh*>(mesh.get())->vertices.end());
						indices.insert(indices.end(), static_cast<Mesh*>(mesh.get())->indices.begin(), static_cast<Mesh*>(mesh.get())->indices.end());
					}
				}
				PRINT_N("vertex size: " << vertexBufferSize);
				PRINT_N("index size: " << indexBufferSize);

				vertexBuffer[1].Initialize(device.Get(), vertexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
				vertexBuffer[1].Get()->SetName(L"Vertex buffer Object 1");

				bufferUploader.Upload((D12Resource*)vertexBuffer[1].GetAddressOf(), vertices.data(), sizeof(Vertex) * vertices.size(), (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

				vertexBufferView[1].BufferLocation = vertexBuffer[1].Get()->GetGPUVirtualAddress();
				vertexBufferView[1].StrideInBytes = sizeof(Vertex);
				vertexBufferView[1].SizeInBytes = vertexBufferSize;

				indexBuffer[1].Initialize(device.Get(), indexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
				indexBuffer[1].Get()->SetName(L"Index buffer Object 1");

				bufferUploader.Upload((D12Resource*)indexBuffer[1].GetAddressOf(), indices.data(), sizeof(UINT32) * indices.size(), (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);

				indexBufferView[1].BufferLocation = indexBuffer[1].Get()->GetGPUVirtualAddress();
				indexBufferView[1].Format = DXGI_FORMAT_R32_UINT;
				indexBufferView[1].SizeInBytes = indexBufferSize;
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

		D3D12_RESOURCE_BARRIER barrier = {};
		{
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.Subresource = 0;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		}

		// deffered pipeline
		{

			defferedPipeline.InitializeDeffered(device.Get());
			defferedPipeline->SetName(L"Deffered pipeline");

			defferedPixelPipeline.InitializeDefferedPixel(device.Get(), L"Shaders/postfx/lights/pixel.hlsl");
			defferedPixelPipeline->SetName(L"Deffered Pixel pipeline");

			for (int i = 0; i < renderTargets; i++) {

				defferedOutputTextures[i].InitializeTexture(device.Get(), rWidth, rHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);

				barrier.Transition.pResource = defferedOutputTextures[i].Get();
				cmdL.GraphicsCmd()->ResourceBarrier(1, &barrier);

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
			viewMatrix = DirectX::XMMatrixLookAtLH({ -3.0f, 10.0f, -10.0f, 0.0f }, // camera pos
				{ 0.0f, 0.0f, 0.0f, 0.0f }, // looking at origin
				{ 0.0f, 1.0f, 0.0f, 0.0f });
			viewProjMatrix = viewMatrix * projectionMatrix;

			PassDataBuffer.Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(PassData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			PassDataBuffer.Get()->SetName(L"PassData buffer");
		}

		// materials
		{
			for (auto& mesh : scene.elements[0].get()->elements) {
				if (dynamic_cast<Mesh*>(mesh.get()) != nullptr) {
					static_cast<Mesh*>(mesh.get())->materialResource.Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(Material)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
					std::wstring name = L"Material for ";
					name.append(mesh.get()->name.begin(), mesh.get()->name.end());
					static_cast<Mesh*>(mesh.get())->materialResource->SetName(name.c_str());
					bufferUploader.Upload((D12Resource*)static_cast<Mesh*>(mesh.get())->materialResource.GetAddressOf(), &static_cast<Mesh*>(mesh.get())->material.albedo, sizeof(Material) * 1, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);
				}
			}
			for (auto& mesh : scene.elements[1].get()->elements) {
				if (dynamic_cast<Mesh*>(mesh.get()) != nullptr) {
					static_cast<Mesh*>(mesh.get())->materialResource.Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(Material)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
					std::wstring name = L"Material for ";
					name.append(mesh.get()->name.begin(), mesh.get()->name.end());
					static_cast<Mesh*>(mesh.get())->materialResource->SetName(name.c_str());
					bufferUploader.Upload((D12Resource*)static_cast<Mesh*>(mesh.get())->materialResource.GetAddressOf(), &static_cast<Mesh*>(mesh.get())->material.albedo, sizeof(Material) * 1, (D12CmdList*)cmdL.GetAddressOf(), (D12CmdQueue*)cmdQ.GetAddressOf(), D3D12_RESOURCE_STATE_INDEX_BUFFER);
				}
			}
		}
		// obj transforms
		{
			for (auto& mesh : scene.elements[0].get()->elements) {
				if (dynamic_cast<Mesh*>(mesh.get()) != nullptr) {
					static_cast<Mesh*>(mesh.get())->transformResource.Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
					std::wstring name = L"Transform for ";
					name.append(mesh.get()->name.begin(), mesh.get()->name.end());
					static_cast<Mesh*>(mesh.get())->transformResource->SetName(name.c_str());
					memcpy(static_cast<Mesh*>(mesh.get())->transformResource.GetCPUMemory(), &static_cast<Mesh*>(mesh.get())->transform, sizeof(ObjectData));
				}
			}
			for (auto& mesh : scene.elements[1].get()->elements) {
				if (dynamic_cast<Mesh*>(mesh.get()) != nullptr) {
					static_cast<Mesh*>(mesh.get())->transformResource.Initialize(device.Get(), Utils::CalcConstBufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
					std::wstring name = L"Transform for ";
					name.append(mesh.get()->name.begin(), mesh.get()->name.end());
					static_cast<Mesh*>(mesh.get())->transformResource->SetName(name.c_str());
					memcpy(static_cast<Mesh*>(mesh.get())->transformResource.GetCPUMemory(), &static_cast<Mesh*>(mesh.get())->transform, sizeof(ObjectData));
				}
			}
		}

		lights[0].position = { 0.0f,0.0f,0.0f };
		lights[0].strength = 1.0f;
		lights[0].direction = { 1.0f,-1.0f,0.0f };

	}

	void Renderer::UpdateDraw(const float dt){

		// update buffers
		{
			PassData passData;

			float shadowmapRadius = 15.0f; 
			DirectX::XMVECTOR sceneCenter = { 0.0f, 0.0f, 0.0f };
			DirectX::XMVECTOR lightPos = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&lights[0].direction), -shadowmapRadius);
			DirectX::XMMATRIX viewLights = DirectX::XMMatrixLookAtLH(lightPos, sceneCenter, {0.0f, 1.0f, 0.0f, 0.0f});
			DirectX::XMMATRIX lightProjMatrix = DirectX::XMMatrixOrthographicLH(20, 20, 0.5f, 200.0f);

			viewMatrix = DirectX::XMMatrixLookAtLH(	{ -4.0f, 7.5f, 4.0f, 0.0f }, // camera pos
													{ 0.0f, 2.0f, 0.0f, 0.0f }, // looking at origin
													{ 0.0f, 1.0f, 0.0f, 0.0f });
			viewProjMatrix = viewMatrix * projectionMatrix;

			passData.viewprojmatrix = viewProjMatrix;
			passData.sceneLight = lights[0];
			passData.lightviewprojmatrix = viewLights * lightProjMatrix;


			memcpy(PassDataBuffer.GetCPUMemory(), &passData, sizeof(PassData));

		}

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
			for (auto& mesh : scene.elements[1].get()->elements) {
				if (dynamic_cast<Mesh*>(mesh.get()) != nullptr) {
					if (static_cast<Mesh*>(mesh.get())->includeInShadowMap) {
						cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(1, static_cast<Mesh*>(mesh.get())->transformResource.Get()->GetGPUVirtualAddress());
						cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(2, static_cast<Mesh*>(mesh.get())->materialResource.Get()->GetGPUVirtualAddress());
						cmdL.GraphicsCmd()->DrawIndexedInstanced(static_cast<Mesh*>(mesh.get())->mesh.indexCount, 1, static_cast<Mesh*>(mesh.get())->mesh.indexOffset, static_cast<Mesh*>(mesh.get())->mesh.vertexOffset, 0);
					}
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
			cmdL.GraphicsCmd()->IASetVertexBuffers(0, 1, &vertexBufferView[0]);
			cmdL.GraphicsCmd()->IASetIndexBuffer(&indexBufferView[0]);
			for (auto& mesh : scene.elements[0].get()->elements) {
				if (dynamic_cast<Mesh*>(mesh.get()) != nullptr) {
					cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(1, static_cast<Mesh*>(mesh.get())->transformResource.Get()->GetGPUVirtualAddress());
					cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(2, static_cast<Mesh*>(mesh.get())->materialResource.Get()->GetGPUVirtualAddress());
					cmdL.GraphicsCmd()->DrawIndexedInstanced(static_cast<Mesh*>(mesh.get())->mesh.indexCount, 1, static_cast<Mesh*>(mesh.get())->mesh.indexOffset, static_cast<Mesh*>(mesh.get())->mesh.vertexOffset, 0);
				}
			}
			cmdL.GraphicsCmd()->IASetVertexBuffers(0, 1, &vertexBufferView[1]);
			cmdL.GraphicsCmd()->IASetIndexBuffer(&indexBufferView[1]);
			for (auto& mesh : scene.elements[1].get()->elements) {
				if (dynamic_cast<Mesh*>(mesh.get()) != nullptr) {
					cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(1, static_cast<Mesh*>(mesh.get())->transformResource.Get()->GetGPUVirtualAddress());
					cmdL.GraphicsCmd()->SetGraphicsRootConstantBufferView(2, static_cast<Mesh*>(mesh.get())->materialResource.Get()->GetGPUVirtualAddress());
					cmdL.GraphicsCmd()->DrawIndexedInstanced(static_cast<Mesh*>(mesh.get())->mesh.indexCount, 1, static_cast<Mesh*>(mesh.get())->mesh.indexOffset, static_cast<Mesh*>(mesh.get())->mesh.vertexOffset, 0);
				}
			}

		}


		D3D12_RESOURCE_BARRIER barrier = {};
		// set swapchain back buffer to render target stage
		{
			barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = swapchain.GetCurrentRT();
			barrier.Transition.Subresource = 0;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			cmdL.GraphicsCmd()->ResourceBarrier(1, &barrier);
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
			barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = swapchain.GetCurrentRT();
			barrier.Transition.Subresource = 0;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			cmdL.GraphicsCmd()->ResourceBarrier(1, &barrier);
		}

		// command list close/execute/present
		{
			cmdL.GraphicsCmd()->Close();
			cmdQ.ExecuteCmdList(cmdL.Get());

			swapchain.Present();
		}

		while (cmdQ.GetFence()->GetCompletedValue() < cmdQ.M_GetCurrentFence()){
			_mm_pause();
		}

		cmdL.ResetCmd();
	}
}