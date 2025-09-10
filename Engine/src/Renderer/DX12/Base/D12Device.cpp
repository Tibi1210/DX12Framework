#include "pch.h"
#include "D12Device.h"

namespace Engine {

	D12Device::D12Device(IDXGIAdapter* pAdapter){
		Initialize(pAdapter);
	}

	void D12Device::Initialize(IDXGIAdapter* pAdapter){

		EVAL_HRES(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&ptr_)), "Error creating D12 device.")
	}
}