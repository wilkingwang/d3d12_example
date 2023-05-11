#pragma

#include <assert.h>
#include <D3D12.h>

#include "Device.h"

enum BufferFlags
{
	BF_NEEDSUAV = 1u << 0,
	BF_NEEDSCPUMEMORY = 1u << 1,
	BF_UNIFORMBUFFER = 1u << 2,
	BF_VERTEXBUFFER = 1u << 3,
	BF_INDEXBUFFER = 1u << 4
};

class ResourceView
{
public:
	void Set(uint32_t size, uint32_t dsvDescriptorSize, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptor)
	{
		this->size = size;
		this->descriptorSize = dsvDescriptorSize;
		this->cpuDescriptor = cpuDescriptor;
		this->gpuDescriptor = gpuDescriptor;
	}

	uint32_t GetSize() const
	{
		return size;
	}

	uint32_t GetDescriptorSize() const
	{
		return descriptorSize;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPU(uint32_t i = 0) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorTmp = cpuDescriptor;
		cpuDescriptorTmp.ptr += i * descriptorSize;
		return cpuDescriptorTmp;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPU(uint32_t i = 0) const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorTmp = gpuDescriptor;
		gpuDescriptorTmp.ptr += i * descriptorSize;
		return gpuDescriptorTmp;
	}

private:
	friend class StaticResourceViewHeapDX12;
	friend class DynamicResourceViewHeapDX12;

	uint32_t size = 0;
	uint32_t descriptorSize = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptor;
};

class RTV : public ResourceView
{

};

class DSV : public ResourceView
{

};

class CBV_SRV_UAV : public ResourceView
{

};

class SAMPLER : public ResourceView
{

};

class StaticResourceViewHeap
{
public:
	BOOL OnCreate(Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t descriptorCount, bool forceCPUVisible = false);
	void OnDestory();

	bool AllocDescriptor(uint32_t size, ResourceView* pResourceView)
	{
		if ((index + size) > descriptorCount)
		{
			assert(!"StaticResourceViewHeapDX12 heap ran of memory, increase its size");
			return false;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE cpuView = pHeap->GetCPUDescriptorHandleForHeapStart();
		cpuView.ptr += index * descriptorElementSize;

		D3D12_GPU_DESCRIPTOR_HANDLE gpuView = {};
		if (pHeap->GetDesc().Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
		{
			gpuView = pHeap->GetGPUDescriptorHandleForHeapStart();
			gpuView.ptr += index * descriptorElementSize;
		}

		index += size;
		pResourceView->Set(size, descriptorElementSize, cpuView, gpuView);

		return true;
	}

	ID3D12DescriptorHeap* GetHeap()
	{
		return pHeap;
	}
private:
	uint32_t index;
	uint32_t descriptorCount;
	uint32_t descriptorElementSize;

	ID3D12DescriptorHeap* pHeap;
};

class ResourceViewHeaps
{
public:
	BOOL OnCreate(Device* pDevice, uint32_t cbvDescriptorCount, 
		uint32_t srvDescriptorCount, uint32_t uavDescriptorCount, 
		uint32_t dsvDescriptorCount, uint32_t rtvDescriptorCount, 
		uint32_t samplerDescriptorCount);

	void OnDestroy();

	bool AllocCBVSRVUAVDescriptor(uint32_t size, CBV_SRV_UAV* pRV)
	{
		return cbvSrvUavHeap.AllocDescriptor(size, pRV);
	}

	bool AllocDSVDescriptor(uint32_t size, DSV* pDSV)
	{
		return dsvHeap.AllocDescriptor(size, pDSV);
	}

	bool AllocRTVDescriptor(uint32_t size, RTV* pRTV)
	{
		rtvHeap.AllocDescriptor(size, pRTV);
	}

	bool AllocSampleDescriptor(uint32_t size, SAMPLER* pRV)
	{
		samplerHeap.AllocDescriptor(size, pRV);
	}

	ID3D12DescriptorHeap* GetDSVHeap()
	{
		return dsvHeap.GetHeap();
	}

	ID3D12DescriptorHeap* GetRTVHeap()
	{
		return rtvHeap.GetHeap();
	}

	ID3D12DescriptorHeap* GetSamplerHeap()
	{
		return samplerHeap.GetHeap();
	}

	ID3D12DescriptorHeap* GetCBVSRVUAVHeap()
	{
		return cbvSrvUavHeap.GetHeap();
	}

private:
	StaticResourceViewHeap dsvHeap;
	StaticResourceViewHeap rtvHeap;
	StaticResourceViewHeap samplerHeap;
	StaticResourceViewHeap cbvSrvUavHeap;
};

class CD3DX12_HEAP_PROPERTIES : public D3D12_HEAP_PROPERTIES
{
public:
	CD3DX12_HEAP_PROPERTIES() = default;

	explicit CD3DX12_HEAP_PROPERTIES(const D3D12_HEAP_PROPERTIES& o) : D3D12_HEAP_PROPERTIES(o)
	{
	}

	CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY cpuPageProperty, D3D12_MEMORY_POOL memPollPreference, UINT creationNodeMask = 1, UINT nodeMask = 1)
	{
		Type = D3D12_HEAP_TYPE_CUSTOM;
		CPUPageProperty = cpuPageProperty;
		MemoryPoolPreference = memPollPreference;
		CreationNodeMask = creationNodeMask;
		VisibleNodeMask = nodeMask;
	}

	explicit CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE type, UINT creationNodeMask = 1, UINT nodeMask = 1)
	{
		Type = type;
		CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		CreationNodeMask = creationNodeMask;
		VisibleNodeMask = nodeMask;
	}

	bool IsCPUAccessible() const
	{
		return Type == D3D12_HEAP_TYPE_UPLOAD || Type == D3D12_HEAP_TYPE_READBACK || (Type == D3D12_HEAP_TYPE_CUSTOM &&
			(CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE || CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_BACK));
	}
};

class CD3DX12_RESOURCE_DESC : public D3D12_RESOURCE_DESC
{
public:
	CD3DX12_RESOURCE_DESC() = default;
	explicit CD3DX12_RESOURCE_DESC(const D3D12_RESOURCE_DESC & o) :
		D3D12_RESOURCE_DESC(o)
	{
	}

	CD3DX12_RESOURCE_DESC(
		D3D12_RESOURCE_DIMENSION dimension,
		UINT64 alignment,
		UINT64 width,
		UINT height,
		UINT16 depthOrArraySize,
		UINT16 mipLevels,
		DXGI_FORMAT format,
		UINT sampleCount,
		UINT sampleQuality,
		D3D12_TEXTURE_LAYOUT layout,
		D3D12_RESOURCE_FLAGS flags)
	{
		Dimension = dimension;
		Alignment = alignment;
		Width = width;
		Height = height;
		DepthOrArraySize = depthOrArraySize;
		MipLevels = mipLevels;
		Format = format;
		SampleDesc.Count = sampleCount;
		SampleDesc.Quality = sampleQuality;
		Layout = layout;
		Flags = flags;
	}

	static inline CD3DX12_RESOURCE_DESC Buffer(const D3D12_RESOURCE_ALLOCATION_INFO& resAllocInfo, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
	{
		return CD3DX12_RESOURCE_DESC(D3D12_RESOURCE_DIMENSION_BUFFER, resAllocInfo.Alignment, resAllocInfo.SizeInBytes,
			1, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, flags);
	}

	static inline CD3DX12_RESOURCE_DESC Buffer(UINT64 width, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, UINT64 alignment = 0)
	{
		return CD3DX12_RESOURCE_DESC(D3D12_RESOURCE_DIMENSION_BUFFER, alignment, width, 1, 1, 1,
			DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, flags);
	}
	static inline CD3DX12_RESOURCE_DESC Tex1D(DXGI_FORMAT format, UINT64 width, UINT16 arraySize = 1, UINT16 mipLevels = 0, 
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN, UINT64 alignment = 0)
	{
		return CD3DX12_RESOURCE_DESC(D3D12_RESOURCE_DIMENSION_TEXTURE1D, alignment, width, 1, arraySize,
			mipLevels, format, 1, 0, layout, flags);
	}

	static inline CD3DX12_RESOURCE_DESC Tex2D( DXGI_FORMAT format, UINT64 width, UINT height, UINT16 arraySize = 1, UINT16 mipLevels = 0, UINT sampleCount = 1,
		UINT sampleQuality = 0, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN, UINT64 alignment = 0)
	{
		return CD3DX12_RESOURCE_DESC(D3D12_RESOURCE_DIMENSION_TEXTURE2D, alignment, width, height, arraySize,
			mipLevels, format, sampleCount, sampleQuality, layout, flags);
	}

	static inline CD3DX12_RESOURCE_DESC Tex3D(DXGI_FORMAT format, UINT64 width, UINT height, UINT16 depth,
		UINT16 mipLevels = 0, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN, UINT64 alignment = 0)
	{
		return CD3DX12_RESOURCE_DESC(D3D12_RESOURCE_DIMENSION_TEXTURE3D, alignment, width, height, depth,
			mipLevels, format, 1, 0, layout, flags);
	}

	inline UINT16 Depth() const
	{
		return (Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D ? DepthOrArraySize : 1);
	}

	inline UINT16 ArraySize() const
	{
		return (Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE3D ? DepthOrArraySize : 1);
	}

	inline UINT8 PlaneCount(_In_ ID3D12Device* pDevice) const
	{
		return D3D12GetFormatPlaneCount(pDevice, Format);
	}

	inline UINT Subresources(_In_ ID3D12Device* pDevice) const
	{
		return MipLevels * ArraySize() * PlaneCount(pDevice);
	}

	inline UINT CalcSubresource(UINT MipSlice, UINT ArraySlice, UINT PlaneSlice)
	{
		return D3D12CalcSubresource(MipSlice, ArraySlice, PlaneSlice, MipLevels, ArraySize());
	}
};

inline bool operator==(const D3D12_RESOURCE_DESC& l, const D3D12_RESOURCE_DESC& r)
{
	return l.Dimension == r.Dimension &&
		l.Alignment == r.Alignment &&
		l.Width == r.Width &&
		l.Height == r.Height &&
		l.DepthOrArraySize == r.DepthOrArraySize &&
		l.MipLevels == r.MipLevels &&
		l.Format == r.Format &&
		l.SampleDesc.Count == r.SampleDesc.Count &&
		l.SampleDesc.Quality == r.SampleDesc.Quality &&
		l.Layout == r.Layout &&
		l.Flags == r.Flags;
}
inline bool operator!=(const D3D12_RESOURCE_DESC& l, const D3D12_RESOURCE_DESC& r)
{
	return !(l == r);
}

class Resource
{
public:
	Resource(Device* pDevice) : pDevice(pDevice)
	{
	}

	BOOL CreateTex2D(DXGI_FORMAT Format, const int width, const int height, const int depthOrArray, const unsigned int flags, const char* name);
	BOOL CreateBuffer(const int structSize, const int structCount, const unsigned int flags, const char* name);
	BOOL CreateResource(CD3DX12_RESOURCE_DESC desc, const unsigned int flags, const char* name);

	BOOL CreateCBV(uint32_t index, CBV_SRV_UAV* pRV);
	BOOL CreateSRV(uint32_t index, ResourceView* pRV);
	BOOL CreateUAV(uint32_t index, ResourceView* pRV);

	void FreeCPUMemory()
	{
		if (pCpuBuffer)
		{
			pCpuBuffer->Release();
		}
	}

	void Free()
	{
		FreeCPUMemory();
		if (pGpuBuffer)
		{
			pGpuBuffer->Release();
		}
	}

private:
	int totalMemSize = 0;
	int totalStructCount = 0;
	int structSize = 0;

	Device*			pDevice = nullptr;
	ID3D12Resource* pCpuBuffer = nullptr;
	ID3D12Resource* pGpuBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	CD3DX12_RESOURCE_DESC resourceDesc;
	std::unique_ptr<ResourceView> pUnorderAccessView;
};