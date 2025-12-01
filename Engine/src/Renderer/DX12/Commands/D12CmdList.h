#pragma once

#include <wrl.h>

namespace Engine {


	class RENDER_API D12CmdList : public Microsoft::WRL::ComPtr<ID3D12CommandList> {

	public:
		D12CmdList() = default;
		~D12CmdList();

		void Initialize(ID3D12Device* pDevice);

		void ResetCmd();

		inline ID3D12GraphicsCommandList* GraphicsCmd() { return graphicsCmdList; }

		void Release();

	private:

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
		ID3D12GraphicsCommandList* graphicsCmdList = nullptr;
	};
}

