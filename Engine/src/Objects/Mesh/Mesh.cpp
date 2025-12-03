#include "pch.h"
#include "Mesh.h"

namespace Engine {

	Mesh::~Mesh(){
		Release();
	}

	void Mesh::Release(){
		transformResource.Release();
		materialResource.Release();
	}


}