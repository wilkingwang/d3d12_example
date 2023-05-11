#include "SwapChain.h"
#include "Helper.h"
#include "FreeSyncHDR.h"

BOOL SwapChain::OnCreate(Device* pDevice, uint32_t nFrameBackBufferCount, HWND hWnd)
{
	this->hWnd = hWnd;
	this->pID3D12Device = pDevice->GetDevice();
	this->nFrameBackBufferCount = nFrameBackBufferCount;

	swapChainFormat = fsHdrGetFormat(DISPLAYMODE_SDR);
	descSwapChain = {};
	descSwapChain.BufferCount = nFrameBackBufferCount;
	descSwapChain.Width = 0;
	descSwapChain.Height = 0;
	descSwapChain.Format = swapChainFormat;
	descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	descSwapChain.SampleDesc.Count = 1;

	CreateDXGIFactory1(IID_PPV_ARGS(&pIDXGIFactory5));

	HRESULT ret = pIDXGIFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &bTearingSupport, sizeof(bTearingSupport));
	if (FAILED(ret))
	{
		return FALSE;
	}

	descSwapChain.Flags = bTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	swapChainFence.OnCreate(pDevice, std::string("swapchain fence").c_str());

	IDXGISwapChain1* pSwapChain1;
	HRESULT ret = pIDXGIFactory5->CreateSwapChainForHwnd(pDirectQueue, hWnd, &descSwapChain, nullptr, nullptr, &pSwapChain1);
	if (FAILED(ret))
	{
		return FALSE;
	}

	ret = pIDXGIFactory5->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(ret))
	{
		return FALSE;
	}

	ret = pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain4), (void**)&pSwapChain);
	if (FAILED(ret))
	{
		return FALSE;
	}

	if (displayMode == DISPLAYMODE_SDR)
	{
		swapChainFormat = ConvertIntoGammaFormat(swapChainFormat);
	}


	// create RTV Heaps
	D3D12_DESCRIPTOR_HEAP_DESC descHeapRtv;
	descHeapRtv.NumDescriptors = descSwapChain.BufferCount;
	descHeapRtv.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descHeapRtv.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descHeapRtv.NodeMask = 0;
	ret = pID3D12Device->CreateDescriptorHeap(&descHeapRtv, IID_PPV_ARGS(&rtvHeaps));
	if (FAILED(ret))
	{
		return FALSE;
	}

	return TRUE;
}

void SwapChain::OnDestroy()
{
	swapChainFence.onDestroy();

	rtvHeaps->Release();
	pSwapChain->Release();
	pIDXGIFactory5->Release();
}