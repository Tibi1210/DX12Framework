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

			D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			bool includeInShadowMap = false;
		};

		struct Material {

			DirectX::XMFLOAT4 albedo = { 1.0f, 0.0f, 0.86f, 1.0f };
			
		};

		struct Light {

			DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
			float strength = 0.0f;
			DirectX::XMFLOAT3 direction = { 0.0f, 0.0f, 0.0f };
			float _padding = 0.0f;

		};

		struct PassData {
			DirectX::XMMATRIX MATRIX_V = DirectX::XMMatrixIdentity();
			DirectX::XMMATRIX MATRIX_P = DirectX::XMMatrixIdentity();
			DirectX::XMMATRIX MATRIX_VP = DirectX::XMMatrixIdentity();
			DirectX::XMMATRIX MATRIX_V_I = DirectX::XMMatrixIdentity();
			DirectX::XMMATRIX MATRIX_P_I = DirectX::XMMatrixIdentity();
			DirectX::XMMATRIX MATRIX_VP_I = DirectX::XMMatrixIdentity();

			DirectX::XMFLOAT3 EYE_POS = { 0.0f, 0.0f, 0.0f };
			DirectX::XMFLOAT2 RENDER_TARGET_SIZE = { 0.0f, 0.0f };
			DirectX::XMFLOAT2 RENDER_TARGET_SIZE_I = { 0.0f, 0.0f };

			float NEAR_Z = 0.0f;
			float FAR_Z = 0.0f;
			float TOTAL_TIME = 0.0f;
			float DELTA_TIME = 0.0f;

			DirectX::XMMATRIX LIGHT_MATRIX_VP = DirectX::XMMatrixIdentity();
			Light sceneLight;
		};

		struct ObjectData {

			DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();

		};

	}

 }