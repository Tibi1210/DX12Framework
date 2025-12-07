#include "pch.h"
#include "DXGIDebug.h"

namespace Engine {

	DXGIDebug DXGIDebug::instance;

	void DXGIDebug::Enable() {

		GetInterface();
		

	}

	void DXGIDebug::GetLiveObjects(){
		Get()->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}

	void DXGIDebug::GetInterface() {

		if (!Get()) {

			EVAL_HRES(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&ptr_)), "Error getting DXGI debug interface.");

		}
	}
}