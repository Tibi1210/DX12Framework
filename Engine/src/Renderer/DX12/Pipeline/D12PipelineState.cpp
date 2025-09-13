#include "pch.h"
#include "D12PipelineState.h"

namespace Engine {

	D12PipelineState::~D12PipelineState() {
		Release();
	}

	void D12PipelineState::Initialize(ID3D12Device* pDevice) {

		rootSignature.Initialize(pDevice);
		shaders[0].Initialize(L"Shaders/vertex.hlsl", HLSLShader::ShaderType::VERTEX);
		shaders[1].Initialize(L"Shaders/pixel.hlsl", HLSLShader::ShaderType::PIXEL);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc = {};
		psDesc.pRootSignature = rootSignature.Get();
		psDesc.VS.pShaderBytecode = shaders[0].GetByteBlob()->GetBufferPointer();
		psDesc.VS.BytecodeLength = shaders[0].GetByteBlob()->GetBufferSize();
		psDesc.PS.pShaderBytecode = shaders[1].GetByteBlob()->GetBufferPointer();
		psDesc.PS.BytecodeLength = shaders[1].GetByteBlob()->GetBufferSize();

		psDesc.BlendState.AlphaToCoverageEnable = false;
		psDesc.BlendState.IndependentBlendEnable = false;
		psDesc.BlendState.RenderTarget[0].BlendEnable = false;
		psDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
		psDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		
		psDesc.SampleMask = 0xFFFFFF;
		psDesc.SampleDesc = { 1,0 };

		psDesc.RasterizerState = {};
		psDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; //back!!!
		psDesc.RasterizerState.FrontCounterClockwise = false;
		psDesc.RasterizerState.DepthClipEnable = true;
		psDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		
		psDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psDesc.DepthStencilState.DepthEnable = true;
		psDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		D3D12_INPUT_ELEMENT_DESC elements[2] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = 2;
		inputLayoutDesc.pInputElementDescs = elements;

		psDesc.InputLayout = inputLayoutDesc;

		psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psDesc.NumRenderTargets = 1;
		psDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		psDesc.NodeMask = 0;
		psDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		EVAL_HRES(pDevice->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(&ptr_)), "Error creating graphics pipeline state.");

	}

	void D12PipelineState::Release() {
		
		rootSignature.Reset();
		shaders[0].Release();
		shaders[1].Release();
		
		if (Get())
		{
			Reset();
		}
	}
}