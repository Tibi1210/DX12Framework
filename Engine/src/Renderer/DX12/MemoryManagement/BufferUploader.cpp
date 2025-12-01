#include "pch.h"
#include "BufferUploader.h"

namespace Engine {

	BufferUploader::~BufferUploader(){
		Release();
	}

	void BufferUploader::Initialize(ID3D12Device* pDevice, const UINT memAlloc){

		internalBuffer.Initialize(pDevice, memAlloc, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
		internalBuffer.Get()->SetName(L"UPLOAD BUFFER");
		internalMemory = internalBuffer.GetCPUMemory();
		maxSize = memAlloc;
	}

	void BufferUploader::Upload(D12Resource* pDest, void* pSource, const UINT size, D12CmdList* pCmdL, D12CmdQueue* pCmdQ, D3D12_RESOURCE_STATES customState){

		ASSERT(size < maxSize);
		memcpy(internalMemory, pSource, size);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pDest->Get();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		
		pCmdL->GraphicsCmd()->ResourceBarrier(1, &barrier);
		pCmdL->GraphicsCmd()->CopyBufferRegion(pDest->Get(), 0, internalBuffer.Get(), 0, size);

		barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pDest->Get();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = customState;
		
		pCmdL->GraphicsCmd()->ResourceBarrier(1, &barrier);

		pCmdL->GraphicsCmd()->Close();
		pCmdQ->ExecuteCmdList(pCmdL->Get());

		while (pCmdQ->GetFence()->GetCompletedValue() < pCmdQ->M_GetCurrentFence()) {
			_mm_pause();
		}

		pCmdL->ResetCmd();
	}

	void BufferUploader::Release(){
		if (internalMemory){
			internalMemory = nullptr;
		}
		internalBuffer.Release();
	}
}