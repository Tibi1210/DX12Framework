#pragma once
#include <wrl.h>

#include "D12RootSignature.h"
#include "HLSLShader.h"

namespace Engine {

	class RENDER_API D12PipelineState : public Microsoft::WRL::ComPtr<ID3D12PipelineState> {

	public:

		D12PipelineState() = default;
		~D12PipelineState();

		void Initialize(ID3D12Device* pDevice);

		void InitializeDeffered(ID3D12Device* pDevice);

		void InitializeShadowMap(ID3D12Device* pDevice);

		void InitializeDefferedPixel(ID3D12Device* pDevice, std::wstring path);

		void InitializeAsTransparent(ID3D12Device* pDevice);

		inline ID3D12RootSignature* GetRootSignature() { return rootSignature.Get(); };

		void Release();


	private:

		D12RootSignature rootSignature;
		HLSLShader shaders[2];

	};

}


