#include "D3D12Framework.h"

D3D12Framework::D3D12Framework(WindowConfig& windowConfig)
{
	iWidth = windowConfig.iWidth;
	iHeight = windowConfig.iHeight;

	nFrameIndex = 0;

	nDXGIFactoryFlags = 0U;
	nRTVDescriptorSize = 0U;

	emRenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	emFetureLevel = D3D_FEATURE_LEVEL_12_1;

	stViewPort.TopLeftX = 0.0f;
	stViewPort.TopLeftY = 0.0f;
	stViewPort.Width = static_cast<float>(windowConfig.iWidth);
	stViewPort.Height = static_cast<float>(windowConfig.iHeight);
	stViewPort.MinDepth = D3D12_MIN_DEPTH;
	stViewPort.MaxDepth = D3D12_MAX_DEPTH;

	stScissortRect.left = 0;
	stScissortRect.top = 0;
	stScissortRect.right = static_cast<LONG>(windowConfig.iWidth);
	stScissortRect.bottom = static_cast<LONG>(windowConfig.iHeight);
}

D3D12Framework::~D3D12Framework()
{

}

BOOL D3D12Framework::CreateIDXGIFactory(HWND hWnd)
{
	HRESULT ret = CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&pIDXGIFactory5));
	if (FAILED(ret))
	{
		return FALSE;
	}

	ret = pIDXGIFactory5->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(ret))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL D3D12Framework::CreateIDXGIDevice(HWND hWnd)
{
	DXGI_ADAPTER_DESC1 tAdapterDesc = {};
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pIDXGIFactory5->EnumAdapters1(adapterIndex, &pIDXGIAdapter1); ++adapterIndex)
	{
		pIDXGIAdapter1->GetDesc1(&tAdapterDesc);

		if (tAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// 跳过软件虚拟设配器设备
			continue;
		}

		if (SUCCEEDED(D3D12CreateDevice(pIDXGIAdapter1.Get(), emFetureLevel, __uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
	}

	HRESULT ret = D3D12CreateDevice(pIDXGIAdapter1.Get(), emFetureLevel, IID_PPV_ARGS(&pID3D12Device4));
	if (FAILED(ret))
	{
		return FALSE;
	}

	outputIDXGIAdapterInfo(hWnd, tAdapterDesc);
	return TRUE;
}

BOOL D3D12Framework::CreateD3D12CommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC tQueueDesc = {};
	tQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HRESULT ret = pID3D12Device4->CreateCommandQueue(&tQueueDesc, IID_PPV_ARGS(&pID3D13CmdQueue));
	if (FAILED(ret))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL D3D12Framework::CreateIDXGISwapChain(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC1 tSwapChainDesc = {};
	tSwapChainDesc.BufferCount = nFrameBackBufCount;
	tSwapChainDesc.Width = iWidth;
	tSwapChainDesc.Height = iHeight;
	tSwapChainDesc.Format = emRenderTargetFormat;
	tSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	tSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	tSwapChainDesc.SampleDesc.Count = 1;

	HRESULT ret = pIDXGIFactory5->CreateSwapChainForHwnd(pID3D13CmdQueue.Get(), hWnd, &tSwapChainDesc, nullptr, nullptr, &pIDXGISwapChain1);
	if (FAILED(ret))
	{
		return FALSE;
	}

	ret = pIDXGISwapChain1.As(&pIDXGISwapChain3);
	if (FAILED(ret))
	{
		return FALSE;
	}

	nFrameIndex = pIDXGISwapChain3->GetCurrentBackBufferIndex();

	return TRUE;
}