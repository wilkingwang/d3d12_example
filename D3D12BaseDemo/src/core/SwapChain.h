#pragma

#include <stdint.h>

#include <D3D12.h>
#include <dxgi1_6.h>

#include "Device.h"
#include "Fence.h"
#include "FreeSyncHDR.h"

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

	Fence										swapChainFence;
	ID3D12DescriptorHeap*						rtvHeaps;
	DXGI_SWAP_CHAIN_DESC1						descSwapChain = {};
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>	cpuViewList;

	DXGI_FORMAT swapChainFormat = DXGI_FORMAT_UNKNOWN;
	DisplayMode displayMode = DisplayMode::DISPLAYMODE_SDR;

	BOOL bTearingSupport = false;
	BOOL bIsFullScreenExclusive = false;
};