#include "pch.h"
#include "Mesh.h"

namespace Engine {

	Mesh::~Mesh(){
		Release();
	}

	void Mesh::Release(){
		PRINT_N("MESH RELEASED!");
		transformResource.Release();
		materialResource.Release();
	}


}