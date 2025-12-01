#include "pch.h"
#include "D12DescriptorHeap.h"

namespace Engine {

	D12DescriptorHeap::~D12DescriptorHeap() {
		Release();
	}

	void D12DescriptorHeap::InitializeDepthHeap(ID3D12Device* pDevice, const unsigned int numDesc) {
		
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.NumDescriptors = numDesc;
		desc.NodeMask = 0;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		EVAL_HRES(pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&ptr_)), "Error creating descriptor heap.");

		incrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		size = numDesc;
	}

	void D12DescriptorHeap::InitializeCBVSRVUAV(ID3D12Device* pDevice, const unsigned int numDesc){

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = numDesc;
		desc.NodeMask = 0;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		EVAL_HRES(pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&ptr_)), "Error creating descriptor heap.");

		incrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		size = numDesc;
	}

	void D12DescriptorHeap::InitializeRTV(ID3D12Device* pDevice, const unsigned int numDesc){

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = numDesc;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;
		EVAL_HRES(pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&ptr_)), "Error creating descriptor heap.");
		
		incrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		size = numDesc;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D12DescriptorHeap::GetCPUHandle(const size_t idx){
		assert(idx < size);

		D3D12_CPU_DESCRIPTOR_HANDLE handle = Get()->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += idx * (size_t)incrementSize;

		return handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE D12DescriptorHeap::GetGPUHandle(const size_t idx){
		assert(idx < size);

		D3D12_GPU_DESCRIPTOR_HANDLE handle = Get()->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += (UINT64)idx * (UINT64)incrementSize;

		return handle;
	}

	void D12DescriptorHeap::Release() {
		if (Get()){
			Reset();
		}
		size = 0;
		incrementSize = 0;
	}
}