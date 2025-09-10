#include "pch.h"
#include "D12CmdQueue.h"

namespace Engine {

	D12CmdQueue::~D12CmdQueue() {
		Release();
	}

	void D12CmdQueue::Initialize(ID3D12Device* pDevice){

		D3D12_COMMAND_QUEUE_DESC desc;
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		EVAL_HRES(pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&ptr_)), "Error creating command quene.");
		EVAL_HRES(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())), "Error creating fence.");

	}

	void D12CmdQueue::ExecuteCmdList(ID3D12CommandList* pCmdL){

		Get()->ExecuteCommandLists(1, (ID3D12CommandList* const*)&pCmdL);
		currentFenceVal++;
		Get()->Signal(fence.Get(), currentFenceVal);
	}

	void D12CmdQueue::Release(){

		if (Get())
		{
			Reset();
		}

		if (fence.Get())
		{
			fence.Reset();
		}
	}

	void D12CmdQueue::FlushQuene(){
		if (Get()) {
			for (unsigned int i = 0; i < G_MAX_SWAPCHAIN_BUFFERS; i++)
			{
				Get()->Signal(fence.Get(), currentFenceVal + i);
			}
		}
	}

}