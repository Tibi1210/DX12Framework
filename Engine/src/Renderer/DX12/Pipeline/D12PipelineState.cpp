#include "pch.h"
#include "D12PipelineState.h"

namespace Engine {

	D12PipelineState::~D12PipelineState() {
		Release();
	}

	void D12PipelineState::Initialize(ID3D12Device* pDevice) {

		rootSignature.Initialize(pDevice);
		shaders[0].Initialize(L"Shaders/base/vertex.hlsl", HLSLShader::ShaderType::VERTEX);
		shaders[1].Initialize(L"Shaders/base/pixel.hlsl", HLSLShader::ShaderType::PIXEL);

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
		psDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; //back!!!
		psDesc.RasterizerState.FrontCounterClockwise = false; // blender
		psDesc.RasterizerState.DepthClipEnable = true;
		psDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		
		psDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		psDesc.DepthStencilState.DepthEnable = true;
		psDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		D3D12_INPUT_ELEMENT_DESC elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
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

	void D12PipelineState::InitializeDeffered(ID3D12Device* pDevice){

		rootSignature.Initialize(pDevice);
		shaders[0].Initialize(L"Shaders/deffered/vertex.hlsl", HLSLShader::ShaderType::VERTEX);
		shaders[1].Initialize(L"Shaders/deffered/pixel.hlsl", HLSLShader::ShaderType::PIXEL);

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
		psDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; //back!!!
		psDesc.RasterizerState.FrontCounterClockwise = false; // blender
		psDesc.RasterizerState.DepthClipEnable = true;
		psDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		psDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		psDesc.DepthStencilState.DepthEnable = true;
		psDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		D3D12_INPUT_ELEMENT_DESC elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = 2;
		inputLayoutDesc.pInputElementDescs = elements;

		psDesc.InputLayout = inputLayoutDesc;
		psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		psDesc.NumRenderTargets = 3;
		psDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		psDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		psDesc.RTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		psDesc.NodeMask = 0;
		psDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		EVAL_HRES(pDevice->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(&ptr_)), "Error creating graphics pipeline state.");

	}

	void D12PipelineState::InitializeShadowMap(ID3D12Device* pDevice){

		rootSignature.Initialize(pDevice);
		shaders[0].Initialize(L"Shaders/shadowmap/vertex.hlsl", HLSLShader::ShaderType::VERTEX);
		//shaders[1].Initialize(L"Shaders/shadowmap/pixel.hlsl", HLSLShader::ShaderType::PIXEL);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc = {};
		psDesc.pRootSignature = rootSignature.Get();
		psDesc.VS.pShaderBytecode = shaders[0].GetByteBlob()->GetBufferPointer();
		psDesc.VS.BytecodeLength = shaders[0].GetByteBlob()->GetBufferSize();
		//psDesc.PS.pShaderBytecode = shaders[1].GetByteBlob()->GetBufferPointer();
		psDesc.PS.pShaderBytecode = nullptr;
		//psDesc.PS.BytecodeLength = shaders[1].GetByteBlob()->GetBufferSize();
		psDesc.PS.BytecodeLength = 0;

		psDesc.BlendState.AlphaToCoverageEnable = false;
		psDesc.BlendState.IndependentBlendEnable = false;
		psDesc.BlendState.RenderTarget[0].BlendEnable = false;
		psDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
		psDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		psDesc.SampleMask = 0xFFFFFF;
		psDesc.SampleDesc = { 1,0 };

		psDesc.RasterizerState = {};
		psDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; //back!!!
		psDesc.RasterizerState.FrontCounterClockwise = false; // blender
		psDesc.RasterizerState.DepthClipEnable = true;
		psDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		psDesc.RasterizerState.DepthBias = 1000;
		psDesc.RasterizerState.DepthBiasClamp = 0.0f;
		psDesc.RasterizerState.SlopeScaledDepthBias = 2.0f;

		psDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psDesc.DepthStencilState.DepthEnable = true;
		psDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		D3D12_INPUT_ELEMENT_DESC elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = 2;
		inputLayoutDesc.pInputElementDescs = elements;

		psDesc.InputLayout = inputLayoutDesc;

		psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psDesc.NumRenderTargets = 0;
		psDesc.NodeMask = 0;
		psDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		EVAL_HRES(pDevice->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(&ptr_)), "Error creating graphics pipeline state.");

	}

	void D12PipelineState::InitializeDefferedPixel(ID3D12Device* pDevice, std::wstring path){
		rootSignature.Initialize(pDevice);
		shaders[0].Initialize(L"Shaders/postfx/vertex.hlsl", HLSLShader::ShaderType::VERTEX);
		shaders[1].Initialize(path.c_str(), HLSLShader::ShaderType::PIXEL);

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
		psDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; //back!!!
		psDesc.RasterizerState.FrontCounterClockwise = false; // blender
		psDesc.RasterizerState.DepthClipEnable = true;
		psDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		psDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		psDesc.DepthStencilState.DepthEnable = false;
		psDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		D3D12_INPUT_ELEMENT_DESC elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = 1;
		inputLayoutDesc.pInputElementDescs = elements;

		psDesc.InputLayout = inputLayoutDesc;

		psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psDesc.NumRenderTargets = 1;
		psDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		psDesc.NodeMask = 0;
		psDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		EVAL_HRES(pDevice->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(&ptr_)), "Error creating graphics pipeline state.");

	}

	void D12PipelineState::InitializeAsTransparent(ID3D12Device* pDevice){
		rootSignature.Initialize(pDevice);
		shaders[0].Initialize(L"Shaders/Shadow/shadow_vertex.hlsl", HLSLShader::ShaderType::VERTEX);
		shaders[1].Initialize(L"Shaders/Shadow/shadow_pixel.hlsl", HLSLShader::ShaderType::PIXEL);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc = {};
		psDesc.pRootSignature = rootSignature.Get();
		psDesc.VS.pShaderBytecode = shaders[0].GetByteBlob()->GetBufferPointer();
		psDesc.VS.BytecodeLength = shaders[0].GetByteBlob()->GetBufferSize();
		psDesc.PS.pShaderBytecode = shaders[1].GetByteBlob()->GetBufferPointer();
		psDesc.PS.BytecodeLength = shaders[1].GetByteBlob()->GetBufferSize();

		psDesc.BlendState.AlphaToCoverageEnable = false;
		psDesc.BlendState.IndependentBlendEnable = false;
		psDesc.BlendState.RenderTarget[0].BlendEnable = true;
		psDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		psDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		psDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		psDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
		psDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


		psDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		psDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		psDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;


		psDesc.SampleMask = 0xFFFFFF;
		psDesc.SampleDesc = { 1,0 };

		psDesc.RasterizerState = {};
		psDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; //back!!!
		psDesc.RasterizerState.FrontCounterClockwise = true; // blender
		psDesc.RasterizerState.DepthClipEnable = true;
		psDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		psDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		psDesc.DepthStencilState.DepthEnable = true;
		psDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		psDesc.DepthStencilState.StencilEnable = true;
		psDesc.DepthStencilState.StencilReadMask = 0xFF;
		psDesc.DepthStencilState.StencilWriteMask = 0xFF;
		psDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
		psDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		psDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
		psDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		psDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
		psDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		psDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
		psDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;

		D3D12_INPUT_ELEMENT_DESC elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
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