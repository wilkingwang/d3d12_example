#pragma
#include <strsafe.h>
#include <tchar.h>

#include <D3D12.h>
#include <dxgi1_6.h>

#include "d3d12x/d3dx12.h"

class Device
{
public:
	Device();
	BOOL OnCreate(bool bValidationEnable, bool bGpuValidationEnable, HWND hWnd);
	BOOL GPUFlush(D3D12_COMMAND_LIST_TYPE queueType);
	BOOL GPUFlush();
	void OnDestory();

	ID3D12Device* GetDevice()
	{
		return pID3D12Device;
	}

	ID3D12CommandQueue* GetGraphicsQueue()
	{
		return pDirectQueue;
	}

	ID3D12CommandQueue* GetComputeQueue()
	{
		return pComputerQueue;
	}

private:
	BOOL createFactory(bool bValidationEnable, bool bGpuValidationEnable, HWND hWnd);
	BOOL createDevice(HWND hWnd);
	BOOL createCommandQueue();

	void outputIDXGIAdapterInfo(HWND hWnd, DXGI_ADAPTER_DESC1& tAdapterDesc)
	{
		TCHAR pszWndTitle[MAX_PATH] = {};

		HRESULT ret = pIDXGIAdapter->GetDesc1(&tAdapterDesc);
		if (FAILED(ret))
		{
			return;
		}

		::GetWindowText(hWnd, pszWndTitle, MAX_PATH);
		StringCchPrintf(pszWndTitle, MAX_PATH, _T("%s (GPU:%s)"), pszWndTitle, tAdapterDesc.Description);
		::SetWindowText(hWnd, pszWndTitle);
	}
private:
	D3D_FEATURE_LEVEL	emFetureLevel;
	IDXGIFactory6*	pIDXGIFactory5 = nullptr;
	IDXGIAdapter1*	pIDXGIAdapter = nullptr;
	ID3D12Device*	pID3D12Device = nullptr;

	ID3D12CommandQueue* pDirectQueue = nullptr;
	ID3D12CommandQueue* pComputerQueue = nullptr;


};