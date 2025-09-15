#include "pch.h"
#include "HLSLShader.h"

namespace Engine {


	HLSLShader::~HLSLShader(){
		Release();
	}

	void HLSLShader::Initialize(const LPCWSTR filename, const ShaderType type){

		UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ALL_RESOURCES_BOUND;

		const char* target = "";

		switch (type){
			case ShaderType::VERTEX:{
				target = "vs_5_1";
				break;
			}
			case ShaderType::PIXEL:{
				target = "ps_5_1";
				break;
			}
			default: {
				PRINT_N("Unsupported shader compilation target."); 
				return;
			}
		}

		ID3DBlob* errorMsg = nullptr;

		HRESULT res = D3DCompileFromFile(filename, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", target, flags, 0, &byteCode, &errorMsg);

		if (res != S_OK){
			PRINT_N("Shader loading error: " << res);
		}
		if (errorMsg){
			PRINT_N("Shader compilation error: " << (const char*)errorMsg->GetBufferPointer());
			return;
		}

		PRINT_W_N("Loaded shader: " << filename);

	}

	void HLSLShader::Release() {
		if (byteCode && byteCode->GetBufferPointer()){
			byteCode->Release();
			byteCode = nullptr;
		}
	}
}