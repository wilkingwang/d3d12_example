#pragma once
#include <tchar.h>

#define WND_CLASS_NAME _T("Direct3D Window Class")
#define WND_TITLE _T("Direct3D Window Sample")

struct WindowConfig
{
	int iWidth;
	int iHeight;
	bool bFullScreen;

	TCHAR* title;
	TCHAR* className;
};