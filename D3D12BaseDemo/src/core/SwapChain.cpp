#include "SwapChain.h"

BOOL SwapChain::OnCreate(Device* pDevice, uint32_t nFrameBackBufferCount, HWND hWnd)
{
	this->hWnd = hWnd;
	this->pID3D12Device = pDevice->GetDevice();
	this->nFrameBackBufferCount = nFrameBackBufferCount;


}

void SwapChain::OnDestroy()
{

}