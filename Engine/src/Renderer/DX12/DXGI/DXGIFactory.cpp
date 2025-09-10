#include "pch.h"
#include "DXGIFactory.h"


namespace Engine {

	using namespace Microsoft::WRL;

	DXGIFactory::DXGIFactory(){

		//turn off debug on prod
		EVAL_HRES(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&ptr_)), "Error creating factory.");


	}

	DXGIAdapter DXGIFactory::GetAdapter(){

		ComPtr<IDXGIFactory6> fact6;
		ComPtr<IDXGIAdapter> adapter;

		if (Get()->QueryInterface(IID_PPV_ARGS(&fact6)) == S_OK){
			EVAL_HRES(fact6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)), "Error defining adapter.");
		}
		else{
			ASSERT(false);
		}

		return DXGIAdapter(adapter.Get());
	}
}