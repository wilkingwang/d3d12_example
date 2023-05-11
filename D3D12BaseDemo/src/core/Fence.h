#include "Device.h"

class Fence
{
public:
	Fence();
	~Fence();

	BOOL OnCreate(Device* pDevice, const char *pDebugName);
	void onDestroy();
	BOOL IssueFence(ID3D12CommandQueue* pCommandQueue);

	BOOL CpuWaitForFence(UINT64 olderFence);
	BOOL GpuWaitForFence(ID3D12CommandQueue* pCommandQueue);

private:
	HANDLE		 hEvent;
	ID3D12Fence* pFence;
	UINT64	   	 fenceCounter;
};