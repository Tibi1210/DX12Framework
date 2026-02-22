#include "pch.h"
#include "Mesh.h"
#include "ModelLoader/ModelLoader.h"

namespace Engine {

	Mesh::~Mesh(){
		Release();
	}

	void Mesh::Load(const char* path){
		ModelLoader modelLoader;
		modelLoader.LoadFBXModels(path, *this);
	}

	void Mesh::Release(){
		for (auto& obj : elements) {
			obj.get()->Release();
		}
	}


}