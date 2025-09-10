#pragma once

#include <wrl.h>

namespace Engine {

	class RENDER_API D12CmdQueue : public Microsoft::WRL::ComPtr<ID3D12CommandQueue>{

	public:
		D12CmdQueue() = default;
		~D12CmdQueue();

		void Initialize(ID3D12Device* pDevice);

		void ExecuteCmdList(ID3D12CommandList* pCmdL);

		void Release();

		void FlushQuene();

		inline ID3D12Fence* GetFence() { return fence.Get(); }
		inline UINT64 M_GetCurrentFence() { return currentFenceVal;}

	private:

		Microsoft::WRL::ComPtr<ID3D12Fence> fence;

		UINT64 currentFenceVal = 0;


	};
}

