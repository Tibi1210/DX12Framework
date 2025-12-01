#include "pch.h"
#include "D12CmdList.h"

namespace Engine {
	D12CmdList::~D12CmdList(){
		Release();
	}

	void D12CmdList::Initialize(ID3D12Device* pDevice) {

		EVAL_HRES(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.GetAddressOf())), "Error creating command allocator.");
		
		EVAL_HRES(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.Get(), nullptr, IID_PPV_ARGS(&ptr_)), "Error creating command list.");

		Get()->QueryInterface(IID_PPV_ARGS(&graphicsCmdList));

	}

	void D12CmdList::ResetCmd(){

		allocator.Get()->Reset();
		GraphicsCmd()->Reset(allocator.Get(), 0);

	}

	void D12CmdList::Release() {

		if (Get() && graphicsCmdList){
			graphicsCmdList->Release();
		}

		if (Get()){
			Reset();
		}
		if (allocator.Get()){
			allocator.Reset();
		}
	}
}