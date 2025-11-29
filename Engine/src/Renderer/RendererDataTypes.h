#pragma once
#include "pch.h"

namespace Engine {

	namespace Render {

		struct Vertex{

			DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
			DirectX::XMFLOAT3 normal = { 0.0f, 0.0f, 0.0f };
			//DirectX::XMFLOAT4 color = { 0.0f, 0.0f, 0.0f, 1.0f };

		};

		struct MeshDataRAW{

			UINT32 vertexCount = 0;
			UINT32 vertexOffset = 0;
			UINT32 indexCount = 0;
			UINT32 indexOffset = 0;


		};

		struct Material {

			DirectX::XMFLOAT4 albedo = { 0.0f, 0.0f, 0.0f, 1.0f };
			
		};

		struct Light {

			DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
			float strength = 0.0f;
			DirectX::XMFLOAT3 direction = { 0.0f, 0.0f, 0.0f };
			float _padding = 0.0f;

		};

		struct PassData {

			DirectX::XMMATRIX viewprojmatrix = DirectX::XMMatrixIdentity();
			Light sceneLight;
		};

		struct ObjectData {

			DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();

		};

	}

 }