#include "Device.h"

Device::Device()
{
	emFetureLevel = D3D_FEATURE_LEVEL_12_1;
}

BOOL Device::OnCreate(bool bValidationEnable, bool bGpuValidationEnable, HWND hWnd)
{
	// D3D12 Debug Layer
	if (bValidationEnable || bGpuValidationEnable)
	{
		ID3D12Debug1* pDebugController;
		if (D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController)) == S_OK)
		{
			if (bValidationEnable || bGpuValidationEnable)
			{
				pDebugController->EnableDebugLayer();
				pDebugController->SetEnableGPUBasedValidation(bGpuValidationEnable);
			}

			pDebugController->Release();
		}
	}

	if (!createFactory(bValidationEnable, bGpuValidationEnable))
	{
		return FALSE;
	}

	pIDXGIFactory5->Release();

	if (!createDevice(hWnd))
	{
		return FALSE;
	}
}

BOOL Device::GPUFlush(D3D12_COMMAND_LIST_TYPE queueType)
{
	ID3D12Fence* pFence;

	HRESULT ret = pID3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
	if (FAILED(ret))
	{
		return FALSE;
	}

	ID3D12CommandQueue* queue = (queueType == D3D12_COMMAND_LIST_TYPE_COMPUTE) ? GetComputeQueue() : GetGraphicsQueue();
	if (FAILED(queue->Signal(pFence, 1)))
	{
		return FALSE;
	}

	HANDLE mHandleFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	pFence->SetEventOnCompletion(1, mHandleFenceEvent);
	WaitForSingleObject(mHandleFenceEvent, INFINITE);
	CloseHandle(mHandleFenceEvent);

	pFence->Release();
}

BOOL Device::GPUFlush()
{
	GPUFlush(D3D12_COMMAND_LIST_TYPE_COMPUTE);
	GPUFlush(D3D12_COMMAND_LIST_TYPE_DIRECT);
}

void Device::OnDestory()
{
	pDirectQueue->Release();
	pComputerQueue->Release();
	pIDXGIAdapter->Release();
	pID3D12Device->Release();
}

BOOL Device::createFactory(bool bValidationEnable, bool bGpuValidationEnable, HWND hWnd)
{
	UINT factoryFlags = 0;

	if (bValidationEnable || bGpuValidationEnable)
	{
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}

	HRESULT ret = CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&pIDXGIFactory5));
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

BOOL Device::createDevice(HWND hWnd)
{
	DXGI_ADAPTER_DESC1 tAdapterDesc = {};
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pIDXGIFactory5->EnumAdapters1(adapterIndex, &pIDXGIAdapter); ++adapterIndex)
	{
		pIDXGIAdapter->GetDesc1(&tAdapterDesc);
		if (tAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// 跳过软件虚拟设配器设备
			continue;
		}

		if (SUCCEEDED(D3D12CreateDevice(pIDXGIAdapter, emFetureLevel, __uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
	}

	HRESULT ret = D3D12CreateDevice(pIDXGIAdapter, emFetureLevel, IID_PPV_ARGS(&pID3D12Device));
	if (FAILED(ret))
	{
		return FALSE;
	}

	outputIDXGIAdapterInfo(hWnd, tAdapterDesc);
	return TRUE;
}

BOOL Device::createCommandQueue()
{
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.NodeMask = 0;

		HRESULT ret = pID3D12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pDirectQueue));
		if (FAILED(ret))
		{
			return FALSE;
		}
	}
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		queueDesc.NodeMask = 0;

		HRESULT ret = pID3D12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pComputerQueue));
		if (FAILED(ret))
		{
			return FALSE;
		}
	}

	return TRUE;
}