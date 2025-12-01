#include "pch.h"
#include "D12Resource.h"

namespace Engine {
	
	D12Resource::~D12Resource(){
		Release();
	}

	D12Resource::D12Resource(D12Resource&& other) noexcept{
		if (other.memory){
			other->Unmap(0, 0);
		}
		if (other.Get()){
			Swap(other);
		}
		if (Get() && other.memory){
			Get()->Map(0, 0, &memory);
			other.memory = nullptr;
		}
	}

	D12Resource& D12Resource::operator=(D12Resource&& other) noexcept{

		if (other.memory) {
			other->Unmap(0, 0);
		}
		if (other.Get()) {
			Swap(other);
		}
		if (Get() && other.memory) {
			Get()->Map(0, 0, &memory);
			other.memory = nullptr;
		}
		return *this;
	}

	void D12Resource::Initialize(ID3D12Device* pDevice, const unsigned int numBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initState){


		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = heapType;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = numBytes ;
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc = {1,0};
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		EVAL_HRES(pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, initState, 0, IID_PPV_ARGS(&ptr_)), "Error creating resource.");

	}

	void D12Resource::InitializeDepthBuffer(ID3D12Device* pDevice, const unsigned int width, const unsigned int height, const DXGI_FORMAT format){
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resDesc.Alignment = 0;
		resDesc.Width = width;
		resDesc.Height = height;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 0;
		resDesc.Format = format;
		resDesc.SampleDesc = { 1,0 };
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = format;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0.0f;

		EVAL_HRES(pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&ptr_)), "Error creating depth resource.");

	}

	void D12Resource::InitializeTexture(ID3D12Device* pDevice, const unsigned int width, const unsigned int height, const DXGI_FORMAT format){

		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resDesc.Alignment = 0;
		resDesc.Width = width;
		resDesc.Height = height;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 0;
		resDesc.Format = format;
		resDesc.SampleDesc = { 1,0 };
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = format;
		clearValue.Color[0] = 0.0f;
		clearValue.Color[1] = 0.0f;
		clearValue.Color[2] = 0.0f;
		clearValue.Color[3] = 0.0f;

		EVAL_HRES(pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COMMON, &clearValue, IID_PPV_ARGS(&ptr_)), "Error creating texture resource.");
		CreateSRV();
	}

	void D12Resource::CreateSRV(){
		
		D3D12_RESOURCE_DESC resDesc = Get()->GetDesc();
		mView.srv.Format = resDesc.Format;
		mView.srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		mView.srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		mView.srv.Texture2D.MipLevels = resDesc.MipLevels;
		mView.srv.Texture2D.MostDetailedMip = 0;
		mView.srv.Texture2D.PlaneSlice = 0;
		mView.srv.Texture2D.ResourceMinLODClamp = 0.0f;


	}

	void* D12Resource::GetCPUMemory(){
		if (!memory){
			Get()->Map(0, 0, &memory);
		}
		return memory;

	}

	void D12Resource::Release() {
		if (memory && Get()){
			Get()->Unmap(0, 0);
			memory = nullptr;
		}
		if (Get()){
			Reset();
		}
	}
}