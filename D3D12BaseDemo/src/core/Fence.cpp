#include "Fence.h"
#include "Helper.h"

Fence::Fence()
{
	hEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
}

Fence::~Fence()
{
	CloseHandle(hEvent);
}

BOOL Fence::OnCreate(Device* pDevice, const char* pDebugName)
{
	fenceCounter = 0;
	
	HRESULT ret = pDevice->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
	if (FAILED(ret))
	{
		return FALSE;
	}

	SetName(pFence, pDebugName);
	return TRUE;
}

void Fence::onDestroy()
{
	pFence->Release();
}

BOOL Fence::IssueFence(ID3D12CommandQueue* pCommandQueue)
{
	fenceCounter++;

	HRESULT ret = pCommandQueue->Signal(pFence, fenceCounter);
	if (FAILED(ret))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Fence::CpuWaitForFence(UINT64 olderFence)
{
	if (fenceCounter > olderFence)
	{
		UINT64 valueToWaitFor = fenceCounter - olderFence;
		if (pFence->GetCompletedValue() <= valueToWaitFor)
		{
			HRESULT ret = pFence->SetEventOnCompletion(valueToWaitFor, hEvent);
			if (FAILED(ret))
			{
				return FALSE;
			}

			WaitForSingleObject(hEvent, INFINITE);
		}
	}

	return TRUE;
}

BOOL Fence::GpuWaitForFence(ID3D12CommandQueue* pCommandQueue)
{
	HRESULT ret = pCommandQueue->Wait(pFence, fenceCounter);
	if (FAILED(ret))
	{
		return FALSE;
	}

	return TRUE;
}