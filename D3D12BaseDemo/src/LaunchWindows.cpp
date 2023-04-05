#include <SDKDDKVer.h>
#include <windows.h>
#include <tchar.h>
#include <wrl.h>
#include <strsafe.h>

#include "core/D3D12Framework.h"
#include "common/WindowConfig.h"

#define WIN32_LEAN_AND_MEAN

using namespace Microsoft;
using namespace Microsoft::WRL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

BOOL StartupWindow(HINSTANCE hInstance, int nCmdShow, WindowConfig *wndConfig, HWND &hWnd)
{
	WNDCLASSEX wndClassEx = {};
	wndClassEx.cbSize = sizeof(WNDCLASSEX);
	wndClassEx.style = CS_GLOBALCLASS;
	wndClassEx.lpfnWndProc = WndProc;
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbWndExtra = 0;
	wndClassEx.hInstance = hInstance;
	wndClassEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClassEx.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wndClassEx.lpszClassName = wndConfig->className;

	RegisterClassEx(&wndClassEx);

	DWORD dwWndStyle = WS_OVERLAPPED | WS_SYSMENU;
	RECT rtWnd = {0, 0, wndConfig->iWidth, wndConfig->iHeight};
	AdjustWindowRect(&rtWnd,dwWndStyle, FALSE);

	INT posX = (GetSystemMetrics(SM_CXSCREEN) - rtWnd.right - rtWnd.left) / 2;
	INT posY = (GetSystemMetrics(SM_CYSCREEN) - rtWnd.bottom - rtWnd.top) / 2;

	hWnd = CreateWindowW(wndConfig->className,
		wndConfig->title,
		dwWndStyle,
		posX,
		posY,
		rtWnd.right - rtWnd.left,
		rtWnd.bottom -rtWnd.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

void HandleEvent()
{
	MSG msg = {};
	DWORD dwRet = 0;
	BOOL bExit = FALSE;
	HANDLE hEventFence = nullptr;

	hEventFence = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (hEventFence == nullptr)
	{
		return;
	}

	SetEvent(hEventFence);
	while (!bExit)
	{
		dwRet = ::MsgWaitForMultipleObjects(1, &hEventFence, FALSE, INFINITE, QS_ALLINPUT);
		switch (dwRet - WAIT_OBJECT_0)
		{
		case 0:
			break;
		case 1:
		{
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (WM_QUIT != msg.message)
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				else
				{
					bExit = TRUE;
				}
			}
		}
		break;
		case WAIT_TIMEOUT:
			break;
		default:
			break;
		}
	}
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
	TCHAR title[] = L"Direct3D Demo";
	TCHAR className[] = L"Direct3D Window Class";
	WindowConfig wndConfig = {1024, 768, FALSE, title, className};
	D3D12Framework d3d12FrameWork(wndConfig);


	if (!StartupWindow(hInstance, nCmdShow, &wndConfig, hWnd))
	{
		return -1;
	}

	if (!d3d12FrameWork.CreateIDXGIFactory(hWnd))
	{
		return -1;
	}

	if (!d3d12FrameWork.CreateIDXGIDevice(hWnd))
	{
		return -1;
	}

	HandleEvent();
	return 0;
}
