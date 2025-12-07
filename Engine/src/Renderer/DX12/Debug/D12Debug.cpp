#include "pch.h"
#include "D12Debug.h"

namespace Engine {

	D12Debug D12Debug::instance;

	void D12Debug::Enable() {

		GetInterface();
		Get()->EnableDebugLayer();

	}

	void D12Debug::GetInterface() {

		if (!Get()) {

			EVAL_HRES(D3D12GetDebugInterface(IID_PPV_ARGS(&ptr_)), "Error getting D12 debug interface.");

		}
	}
}