#include "pch.h"
#include "D12RootSignature.h"


namespace Engine {

	D12RootSignature::~D12RootSignature(){
		Release();

	}

	void D12RootSignature::Initialize(ID3D12Device* pDevice){

		D3D12_ROOT_PARAMETER rootParams[2];
		rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParams[0].Descriptor.ShaderRegister = 0;
		rootParams[0].Descriptor.RegisterSpace = 0;
		rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParams[1].Descriptor.ShaderRegister = 1;
		rootParams[1].Descriptor.RegisterSpace = 0;
		rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = {};
		rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
		rsDesc.Desc_1_0.pParameters = rootParams;
		rsDesc.Desc_1_0.NumParameters = 2;
		rsDesc.Desc_1_0.NumStaticSamplers = 0;
		rsDesc.Desc_1_0.pStaticSamplers = 0;
		rsDesc.Desc_1_0.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ID3DBlob* serializedRS = nullptr;
		ID3DBlob* errorMsg = nullptr;

		EVAL_HRES(D3D12SerializeVersionedRootSignature(&rsDesc, &serializedRS, &errorMsg), "Error serializing root signature.");
		
		if (errorMsg) {
			PRINT_N("Root signature serialization error: " << (const char*)errorMsg->GetBufferPointer());
		}

		EVAL_HRES(pDevice->CreateRootSignature(0, serializedRS->GetBufferPointer(), serializedRS->GetBufferSize(), IID_PPV_ARGS(&ptr_)), "Error creating root signature.");


	}

	void D12RootSignature::Release(){
		if (Get()){
			Reset();
		}
	}

}