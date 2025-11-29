#include "pch.h"
#include "ModelLoader.h"

#include <fbxsdk.h>

namespace Engine {

	FbxVector4 GetNormal(FbxGeometryElementNormal* normal, const UINT32 faceIdx) {

		if (normal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex){
			if (normal->GetReferenceMode() == FbxGeometryElement::eDirect) {
				return normal->GetDirectArray().GetAt(faceIdx);
			}else {
				const int idx = normal->GetIndexArray().GetAt(faceIdx);
				return normal->GetDirectArray().GetAt(idx);
			}
		}else {
			PRINT_N("Only face index normal handling implementetd.");
		}
		
		return FbxVector4();
	}

	bool CompareVertex(Render::Vertex& a, Render::Vertex& b) {
		if (a.position.x == b.position.x && a.position.y == b.position.y && a.position.z == b.position.z){
			if (a.normal.x == b.normal.x && a.normal.y == b.normal.y && a.normal.z == b.normal.z) {
				return true;
			}
		}
		return false;
	}

	void ModelLoader::LoadFBXModel(const char* path, std::vector<Render::Vertex>& outVertices, std::vector<UINT32>& outIndices, std::vector<Render::MeshDataRAW>& outMeshes){
		FbxManager* manager = FbxManager::Create();
		FbxIOSettings* settings = FbxIOSettings::Create(manager, IOSROOT);
		manager->SetIOSettings(settings);
		FbxImporter* importer = FbxImporter::Create(manager, "FBX importer");
		if (!importer->Initialize(path, -1, manager->GetIOSettings())){
			PRINT_N("Model loading failed!");
			PRINT_N("ERROR code: " << importer->GetStatus().GetErrorString());
		}
		else{
			PRINT_N("Model loaded: " << path);
		}

		FbxScene* scene = FbxScene::Create(manager, "ModelLoadingScene");

		importer->Import(scene);
		importer->Destroy();
		
		std::vector<Render::Vertex> totalMeshVertices;
		std::vector<UINT32> totalMeshIndices;

		size_t vertexOffset = 0;
		size_t indexOffset = 0;


		for (int geometry = 0; geometry < scene->GetGeometryCount(); geometry++){
			
			FbxMesh* mesh = static_cast<FbxMesh*>(scene->GetGeometry(geometry));
			PRINT_N("Geometry #" << geometry + 1);
			PRINT_N("Vertex count: " << mesh->GetControlPointsCount());
			PRINT_N("Index count: " << (mesh->GetPolygonCount() * mesh->GetPolygonSize(0)));
			
			std::vector<Render::Vertex> meshVertices;
			std::vector<UINT32> meshIndices;

			const int faceCount = mesh->GetPolygonCount();
			const int ControlPointCount = mesh->GetControlPointsCount();
			FbxVector4* ControlPoints = mesh->GetControlPoints();

			unsigned int vtx = 0;

			for (int face = 0; face < faceCount; face++){
				const int faceVertexCount = mesh->GetPolygonSize(face);

				for (int faceVertex = 0; faceVertex < faceVertexCount; faceVertex++){

					const int controlPointIdx = mesh->GetPolygonVertex(face, faceVertex);
					const FbxVector4& positionData = ControlPoints[controlPointIdx];
					Render::Vertex vertex;
					vertex.position = { (float)positionData[0], (float)positionData[1], (float)positionData[2] };
					const int normalCount = mesh->GetElementNormalCount();

					for (int normal = 0; normal < normalCount; normal++){
						FbxGeometryElementNormal* pointNormal = mesh->GetElementNormal(normal);
						const FbxVector4& normalData = GetNormal(pointNormal, vtx);
						vertex.normal = { (float)normalData[0], (float)normalData[1], (float)normalData[2] };
					}

					const size_t meshVertexCount = meshVertices.size();
					size_t i = 0;
					for (i = 0; i < meshVertexCount; i++){
						if (CompareVertex(vertex, meshVertices[i])){
							break;
						}
					}
					if (i == meshVertexCount){
						meshVertices.emplace_back(vertex);
					}
					meshIndices.push_back(i);
					vtx++;
				}
				
			}
			Render::MeshDataRAW rawMesh;
			rawMesh.vertexCount = meshVertices.size();
			rawMesh.vertexOffset = vertexOffset;
			rawMesh.indexCount = meshIndices.size();
			rawMesh.indexOffset = indexOffset;
			outMeshes.emplace_back(rawMesh);

			vertexOffset += rawMesh.vertexCount;
			indexOffset += rawMesh.indexCount;

			totalMeshVertices.insert(totalMeshVertices.end(), meshVertices.begin(), meshVertices.end());
			totalMeshIndices.insert(totalMeshIndices.end(), meshIndices.begin(), meshIndices.end());



		}
			outVertices = std::move(totalMeshVertices);
			outIndices = std::move(totalMeshIndices);
			PRINT_N("Model loaded!");
	}

}