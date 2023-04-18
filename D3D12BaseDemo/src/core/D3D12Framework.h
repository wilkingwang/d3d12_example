#pragma once
#include <strsafe.h>

#include <dxgi1_6.h>
// directx math lib
#include <DirectXMath.h>

#include <d3dx12.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif

// extend of d3d12
#include <d3dx12.h>

#include "../common/WindowConfig.h"

using namespace DirectX;
using namespace Microsoft;
using namespace Microsoft::WRL;

const UINT nFrameBackBufCount = 3u;

enum FWBufferFlags
{
	FW_BF_NEEDSUAV = 1u << 0,			// 
	FW_BF_NEEDSCPUMEMORY = 1u << 1,		// 
	FW_BF_UNIFORMBUFFER = 1u << 2,		// 
	FW_BF_VERTEXBUFFER = 1u << 3,		// 
	FW_BF_INDEXBUFFER = 1u << 4			// 
};

class D3D12ResourceEx
{
	D3D12ResourceEx();
};

class D3D12Framework 
{
public:
	D3D12Framework(WindowConfig &windowConfig);
	~D3D12Framework();

	BOOL CreateIDXGIFactory(HWND hWnd);
	BOOL CreateIDXGIDevice(HWND hWnd);

	BOOL CreateD3D12CommandQueue();
	BOOL CreateIDXGISwapChain(HWND hWnd);

	BOOL CreateDescriptorHeap();
	BOOL CreateRenderTargetView();

	BOOL CreateRootSignature();

	BOOL CreateCommandList();

	BOOL CompileFromFile(const std::string &shaderName, const std::string &entryPoint, const std::string &target, UINT iCompileFlags, ComPtr<ID3D10Blob>& pID3DBlobShader);

	BOOL CreateGraphicsPipelineState(D3D12_GRAPHICS_PIPELINE_STATE_DESC *psoDesc);

	BOOL CreateResource(D3D12_RESOURCE_DESC desc, const unsigned int flags);

private:
	void outputIDXGIAdapterInfo(HWND hWnd, DXGI_ADAPTER_DESC1 &tAdapterDesc)
	{
		TCHAR pszWndTitle[MAX_PATH] = {};

		HRESULT ret = pIDXGIAdapter1->GetDesc1(&tAdapterDesc);
		if (FAILED(ret))
		{
			return;
		}

		::GetWindowText(hWnd, pszWndTitle, MAX_PATH);
		StringCchPrintf(pszWndTitle, MAX_PATH, _T("%s (GPU:%s)"), pszWndTitle, tAdapterDesc.Description);
		::SetWindowText(hWnd, pszWndTitle);
	}

private:
	int iWidth;
	int iHeight;

	UINT nFrameIndex;

	UINT nDXGIFactoryFlags;
	UINT nRTVDescriptorSize;

	DXGI_FORMAT emRenderTargetFormat;

	D3D12_VIEWPORT		stViewPort;
	D3D12_RECT			stScissortRect;
	D3D_FEATURE_LEVEL	emFetureLevel;

	ComPtr<IDXGIFactory5>		pIDXGIFactory5;
	ComPtr<IDXGIAdapter1>		pIDXGIAdapter1;
	ComPtr<ID3D12Device4>		pID3D12Device4;
	ComPtr<ID3D12CommandQueue>	pID3D13CmdQueue;

	ComPtr<IDXGISwapChain1>		pIDXGISwapChain1;
	ComPtr<IDXGISwapChain3>		pIDXGISwapChain3;

	ComPtr<ID3D12DescriptorHeap>	pID3D12RTVHeap;
	ComPtr<ID3D12RootSignature>		pID3D12RootSignature;
	ComPtr<ID3D12PipelineState>		pID3D12PipelineState;
	ComPtr<ID3D12Resource>			pID3D12RenderTargets[nFrameBackBufCount];

	ComPtr<ID3D12Fence>					pID3D12Fence;
	ComPtr<ID3D12Resource>				pID3D12VertexBuffer;
	ComPtr<ID3D12CommandAllocator>		pID3D12CmdAllocator;
	ComPtr<ID3D12GraphicsCommandList>	pID3D12GraphicsCmdList;
};