#pragma

#include <stdint.h>

#include <D3D12.h>
#include <dxgi1_6.h>

#include "Device.h"

class SwapChain
{
public:
	BOOL OnCreate(Device* pDevice, uint32_t nFrameBackBufferCount, HWND hWnd);
	void OnDestroy();

private:
	HWND hWnd;
	uint32_t nFrameBackBufferCount;

	IDXGIFactory6*		pIDXGIFactory5 = nullptr;
	ID3D12Device*		pID3D12Device = nullptr;
	IDXGISwapChain4*	pSwapChain = nullptr;
	ID3D12CommandQueue* pDirectQueue = nullptr;

	DXGI_FORMAT m_swapChainFormat = DXGI_FORMAT_UNKNOWN;
};