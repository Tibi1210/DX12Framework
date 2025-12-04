#pragma once
#include "Objects/Object.h"
#include "Renderer/RendererDataTypes.h"
#include "../../Renderer/DX12/Resources/D12Resource.h"

namespace Engine {

	class Mesh : public Object{

	public:
		inline Mesh() { this->id = 1; };

		~Mesh();

		Render::MeshDataRAW mesh;
		std::vector<Render::Vertex> vertices;
		std::vector<UINT32> indices;

		Render::Material material;
		D12Resource materialResource = D12Resource();

		Render::ObjectData transform;
		D12Resource transformResource = D12Resource();

		bool includeInShadowMap = false;


		void Release() override;

	private:
	};


}

