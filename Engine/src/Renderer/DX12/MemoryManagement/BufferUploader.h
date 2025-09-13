#pragma once
#include "../Resources/D12Resource.h"
#include "../Commands/D12CmdList.h"
#include "../Commands/D12CmdQueue.h"

namespace Engine {

	// TODO: for runtime uploads separate cmdQ and cmdL required (ring/linear buffer)
	class RENDER_API BufferUploader{

	public:
		BufferUploader() = default;
		~BufferUploader();

		void Initialize(ID3D12Device* pDevice, const UINT memAlloc);

		void Upload(D12Resource* pDest, void* pSource, const UINT size, D12CmdList* pCmdL, D12CmdQueue* pCmdQ, D3D12_RESOURCE_STATES customState = D3D12_RESOURCE_STATE_COMMON);

		void Release();

	private:

		D12Resource internalBuffer;

		void* internalMemory = nullptr;

		UINT maxSize = 0;

	};

}

