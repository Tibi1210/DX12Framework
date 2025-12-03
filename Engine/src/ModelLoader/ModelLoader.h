#pragma once

#include <vector>
#include "../Renderer/RendererDataTypes.h"
#include "../Objects/Mesh/Mesh.h"

namespace Engine {

	class RENDER_API ModelLoader{

	public:
		ModelLoader() = default;

		void LoadFBXModel(const char* path, std::vector<Render::Vertex>& outVertices, std::vector<UINT32>& outIndices, std::vector<std::unique_ptr<Object>>& outMeshObjs);

	private:


	};

}
