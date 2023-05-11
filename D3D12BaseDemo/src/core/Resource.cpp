#include "Resource.h"

BOOL ResourceViewHeaps::OnCreate(Device* pDevice, uint32_t cbvDescriptorCount,
	uint32_t srvDescriptorCount, uint32_t uavDescriptorCount,
	uint32_t dsvDescriptorCount, uint32_t rtvDescriptorCount,
	uint32_t samplerDescriptorCount)
{
	dsvHeap.OnCreate(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, dsvDescriptorCount);
	rtvHeap.OnCreate(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rtvDescriptorCount);
	samplerHeap.OnCreate(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, samplerDescriptorCount);
	cbvSrvUavHeap.OnCreate(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, cbvDescriptorCount + srvDescriptorCount + uavDescriptorCount);
}

void ResourceViewHeaps::OnDestroy()
{
	dsvHeap.OnDestory();
	rtvHeap.OnDestory();
	samplerHeap.OnDestory();
	cbvSrvUavHeap.OnDestory();
}

BOOL Resource::CreateTex2D(DXGI_FORMAT format, const int width, const int height, const int depthOrArray, const unsigned int flags, const char* name)
{
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, depthOrArray, 1);
	CreateResource(desc, flags, name);
}

BOOL Resource::CreateBuffer(const int structSize, const int structCount, const unsigned int flags, const char* name)
{
	this->structSize = structSize;
	this->totalStructCount = structCount;
	this->totalMemSize = structSize * structCount;

	if (flags & BF_UNIFORMBUFFER)
	{
		if (totalMemSize % 256)
		{
			totalMemSize += 256 - totalMemSize % 256;
		}
	}

	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(totalMemSize);
	CreateResource(desc, flags, name);
}

BOOL Resource::CreateResource(CD3DX12_RESOURCE_DESC desc, const unsigned int flags, const char* name)
{
	wchar_t  uniName[1024];
	swprintf(uniName, 1024, L"%S", name);
	D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE;
	D3D12_RESOURCE_STATES resourceState = (D3D12_RESOURCE_STATES)D3D12_RESOURCE_STATE_COPY_DEST;

	if (flags & BF_NEEDSUAV)
	{
		resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		resourceState = (D3D12_RESOURCE_STATES)D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	}

	desc.Flags = resourceFlags;
	this->resourceDesc = desc;

	if (flags & BF_NEEDSCPUMEMORY)
	{
		CD3DX12_RESOURCE_DESC cpuResourceDesc = desc;
		cpuResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		HRESULT ret = pDevice->GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 
			D3D12_HEAP_FLAG_NONE, &cpuResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pCpuBuffer));
		if (FAILED(ret))
		{
			return FALSE;
		}

		pCpuBuffer->SetName(uniName);
	}

	HRESULT ret = pDevice->GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE, &desc, resourceState, nullptr, IID_PPV_ARGS(&pGpuBuffer));
	if (FAILED(ret))
	{
		return FALSE;
	}

	pGpuBuffer->SetName(uniName);

	if (flags & BF_INDEXBUFFER)
	{
		indexBufferView.BufferLocation = pGpuBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		indexBufferView.SizeInBytes = totalMemSize;
	}
	
	return TRUE;
}

BOOL Resource::CreateCBV(uint32_t index, CBV_SRV_UAV* pRV)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = pGpuBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = totalMemSize;

	pDevice->GetDevice()->CreateConstantBufferView(&cbvDesc, pRV->GetCPU(index));
}

BOOL Resource::CreateSRV(uint32_t index, ResourceView* pRV)
{

}

BOOL Resource::CreateUAV(uint32_t index, ResourceView* pRV)
{

}