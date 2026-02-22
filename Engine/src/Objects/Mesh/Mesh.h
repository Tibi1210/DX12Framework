#pragma once
#include "Objects/Object.h"
#include "Renderer/RendererDataTypes.h"
#include "Renderer/DX12/Resources/D12Resource.h"


namespace Engine {

	class Mesh : public Object{

	public:
		inline Mesh() { this->id = 1; };

		~Mesh();

		size_t vertexBufferSize = 0;
		size_t indexBufferSize = 0;
		std::vector<Render::Vertex> vertices;
		std::vector<UINT32> indices;

		std::vector<Render::MeshDataRAW> geometries;
		std::vector<Render::Material> materials;
		std::vector<Render::ObjectData> datas;


		void Load(const char* path);

		void Release() override;

	private:
	};


}

