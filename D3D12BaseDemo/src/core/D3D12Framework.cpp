#include "../utils/utils.h"
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

BOOL D3D12Framework::CreateDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC tRTVHeacDesc = {};
	
	tRTVHeacDesc.NumDescriptors = nFrameBackBufCount;
	tRTVHeacDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	tRTVHeacDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT ret = pID3D12Device4->CreateDescriptorHeap(&tRTVHeacDesc, IID_PPV_ARGS(&pID3D12RTVHeap));
	if (FAILED(ret))
	{
		return FALSE;
	}

	nRTVDescriptorSize = pID3D12Device4->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	return TRUE;
}

BOOL D3D12Framework::CreateRenderTargetView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE tRTVHandle = pID3D12RTVHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < nFrameBackBufCount; i++)
	{
		HRESULT ret = pIDXGISwapChain3->GetBuffer(i, IID_PPV_ARGS(&pID3D12RenderTargets[i]));
		if (FAILED(ret))
		{
			return FALSE;
		}

		pID3D12Device4->CreateRenderTargetView(pID3D12RenderTargets[i].Get(), nullptr, tRTVHandle);
		tRTVHandle.ptr += nRTVDescriptorSize;
	}

	return TRUE;
}

BOOL D3D12Framework::CreateRootSignature()
{
	ComPtr<ID3DBlob> pID3DSignatureBlob;
	ComPtr<ID3DBlob> pId3DErrorBlob;
	D3D12_ROOT_SIGNATURE_DESC tRootSignatureDesc = {0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT};

	HRESULT ret = D3D12SerializeRootSignature(&tRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pID3DSignatureBlob, &pId3DErrorBlob);
	if (FAILED(ret))
	{
		return FALSE;
	}

	ret = pID3D12Device4->CreateRootSignature(0, pID3DSignatureBlob->GetBufferPointer(), 
		pID3DSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&pID3D12RootSignature));
	if (FAILED(ret))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL D3D12Framework::CreateCommandList()
{
	HRESULT ret = pID3D12Device4->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pID3D12CmdAllocator));
	if (FAILED(ret))
	{
		return FALSE;
	}

	ret = pID3D12Device4->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, 
		pID3D12CmdAllocator.Get(), pID3D12PipelineState.Get(), IID_PPV_ARGS(&pID3D12GraphicsCmdList));
	if (FAILED(ret))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL D3D12Framework::CompileFromFile(const std::string &shaderName, const std::string &entryPoint, const std::string &target, UINT iCompileFlags, ComPtr<ID3D10Blob> &pID3DBlobShader)
{
	TCHAR shaderFileName[MAX_PATH] = {};

	string2tchar(shaderName, shaderFileName);

	HRESULT ret = D3DCompileFromFile(shaderFileName, nullptr, nullptr, entryPoint, target, iCompileFlags, 0, &pID3DBlobShader, nullptr);
	if (FAILED(ret))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL D3D12Framework::CreateGraphicsPipelineState(D3D12_GRAPHICS_PIPELINE_STATE_DESC *psoDesc)
{
	HRESULT ret = pID3D12Device4->CreateGraphicsPipelineState(psoDesc, IID_PPV_ARGS(&pID3D12PipelineState));
	if (FAILED(ret))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL D3D12Framework::CreateResource(D3D12_RESOURCE_DESC desc, const unsigned int flags)
{
	D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE;

	if (flags & FW_BF_NEEDSUAV)
	{
		resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	desc.Flags = resourceFlags;


	return TRUE;
}