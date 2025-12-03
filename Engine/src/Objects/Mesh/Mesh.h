#pragma once
#include "Objects/Object.h"
#include "Renderer/RendererDataTypes.h"

namespace Engine {

	class Mesh : public Object{

	public:
		Mesh() = default;

		Render::MeshDataRAW mesh;

		void Release() override;

	private:
	};


}

