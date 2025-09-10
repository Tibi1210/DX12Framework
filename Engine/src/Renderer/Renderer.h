#pragma once

#include "EngineBase.h"
#include <Windows.h>

#include "DX12/DXGI/DXGISwapChain.h"

#include "DX12/Commands/D12CmdQueue.h"
#include "DX12/Commands/D12CmdList.h"

#include "DX12/Base/D12Device.h"

#include "DX12/Resources/D12Resource.h"

namespace Engine {

	class RENDER_API Renderer{

	public:
		Renderer() = default;
		~Renderer();

		void Initialize(HWND hwnd, const UINT width, const UINT height);

		void UpdateDraw();


		void Release();

	private:

		D12Device device;
		D12CmdQueue cmdQ;
		D12CmdList cmdL;

		UINT rWidth, rHeight = 0;

		DXGISwapChain swapchain;

		D12Resource dynamicVertexBuffer;
	};
}

