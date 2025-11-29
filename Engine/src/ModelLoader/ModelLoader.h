#pragma once

#include <vector>
#include "../Renderer/RendererDataTypes.h"

namespace Engine {

	class RENDER_API ModelLoader{

	public:
		ModelLoader() = default;

		void LoadFBXModel(const char* path, std::vector<Render::Vertex>& outVertices, std::vector<UINT32>& outIndices, std::vector<Render::MeshDataRAW>& outMeshes);

	private:


	};

}
