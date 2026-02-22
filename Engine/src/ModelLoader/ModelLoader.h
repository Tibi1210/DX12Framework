#pragma once

#include "Renderer/RendererDataTypes.h"
#include "Objects/Mesh/Mesh.h"

namespace Engine {

	class RENDER_API ModelLoader{

	public:
		ModelLoader() = default;

		void LoadFBXModels(const char* path, Mesh& outMeshObj);

	private:


	};

}
