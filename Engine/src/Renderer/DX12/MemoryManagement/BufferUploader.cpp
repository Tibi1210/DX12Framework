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

		CD3DX12_RESOURCE_BARRIER transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pDest->Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST);
		pCmdL->GraphicsCmd()->ResourceBarrier(1, &transitionBarrier);

		pCmdL->GraphicsCmd()->CopyBufferRegion(pDest->Get(), 0, internalBuffer.Get(), 0, size);

		transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pDest->Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			customState);
		pCmdL->GraphicsCmd()->ResourceBarrier(1, &transitionBarrier);

		pCmdL->GraphicsCmd()->Close();
		pCmdQ->ExecuteCmdList(pCmdL->Get());

		if (pCmdQ->GetFence()->GetCompletedValue() < pCmdQ->M_GetCurrentFence())
		{
			HANDLE eventHandle = CreateEventEx(nullptr, L"FenceCompletedEventHandler", false, EVENT_ALL_ACCESS);
			EVAL_HRES(pCmdQ->GetFence()->SetEventOnCompletion(pCmdQ->M_GetCurrentFence(), eventHandle), "Error completing fence.");
			if (eventHandle != nullptr)
			{
				WaitForSingleObject(eventHandle, INFINITE);
				CloseHandle(eventHandle);
			}
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