#include "pch.h"
#include "D12RootSignature.h"


namespace Engine {

	D12RootSignature::~D12RootSignature(){
		Release();

	}

	void D12RootSignature::Initialize(ID3D12Device* pDevice){

		D3D12_ROOT_PARAMETER rootParams[4];
		//pass data
		rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParams[0].Descriptor.ShaderRegister = 0;
		rootParams[0].Descriptor.RegisterSpace = 0;
		rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		//transforms
		rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParams[1].Descriptor.ShaderRegister = 1;
		rootParams[1].Descriptor.RegisterSpace = 0;
		rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		//material
		rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParams[2].Descriptor.ShaderRegister = 2;
		rootParams[2].Descriptor.RegisterSpace = 0;
		rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_DESCRIPTOR_RANGE descRanges[1];
		descRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descRanges[0].BaseShaderRegister = 0;
		descRanges[0].RegisterSpace = 0;
		descRanges[0].NumDescriptors = 5;
		descRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		rootParams[3].DescriptorTable.pDescriptorRanges = descRanges;
		rootParams[3].DescriptorTable.NumDescriptorRanges = 1;

		D3D12_STATIC_SAMPLER_DESC samplerDesc[3];
		samplerDesc[0].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc[0].ShaderRegister = 0;
		samplerDesc[0].RegisterSpace = 1;
		samplerDesc[0].MinLOD = 0;
		samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc[0].MipLODBias = 0.0f;
		samplerDesc[0].MaxAnisotropy = 16;
		samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		samplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[1].ShaderRegister = 0;
		samplerDesc[1].RegisterSpace = 0;
		samplerDesc[1].MinLOD = 0;
		samplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc[1].MipLODBias = 0.0f;
		samplerDesc[1].MaxAnisotropy = 1;
		samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		samplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


		samplerDesc[2].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[2].ShaderRegister = 1;
		samplerDesc[2].RegisterSpace = 0;
		samplerDesc[2].MinLOD = 0;
		samplerDesc[2].MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc[2].MipLODBias = 0.0f;
		samplerDesc[2].MaxAnisotropy = 1;
		samplerDesc[2].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		samplerDesc[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;




		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = {};
		rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
		rsDesc.Desc_1_0.pParameters = rootParams;
		rsDesc.Desc_1_0.NumParameters = 4;
		rsDesc.Desc_1_0.NumStaticSamplers = 2;
		rsDesc.Desc_1_0.pStaticSamplers = samplerDesc;
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