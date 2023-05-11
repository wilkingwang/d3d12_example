#include <stdio.h>
#include <wchar.h>
#include <assert.h>

#include "Helper.h"

void SetName(ID3D12Object* pObj, const char* name)
{
	if (name != NULL)
	{
		SetName(pObj, std::string(name));
	}
}

void SetName(ID3D12Object* pObj, const std::string& name)
{
	assert(pObj);
	wchar_t nameBuffer[128];
	
	if (name.size() >= 128)
	{
		swprintf(nameBuffer, 128, L"%S", name.substr(name.size() - 127, name.size()).c_str());
	}
	else
	{
		swprintf(nameBuffer, name.size() + 1, L"%S", name.c_str());
	}

	pObj->SetName(nameBuffer);
}

DXGI_FORMAT ConvertIntoGammaFormat(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
	case DXGI_FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM_SRGB;
	case DXGI_FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM_SRGB;
	case DXGI_FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM_SRGB;
	case DXGI_FORMAT_BC7_UNORM:
		return DXGI_FORMAT_BC7_UNORM_SRGB;
	}

	return format;
}