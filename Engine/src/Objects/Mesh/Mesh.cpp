#include "pch.h"
#include "Mesh.h"

namespace Engine {

	Mesh::~Mesh(){
		Release();
	}

	void Mesh::Load(const char* path){
		ModelLoader modelLoader;
		modelLoader.LoadFBXModels(path, elements);
	}

	void Mesh::Release(){
		transformResource.Release();
		materialResource.Release();
		for (auto& obj : elements) {
			obj.get()->Release();
		}
	}


}