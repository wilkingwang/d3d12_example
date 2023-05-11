#pragma once
#include <dxgi1_6.h>

enum DisplayMode
{
	DISPLAYMODE_SDR,
	DISPLAYMODE_FSHDR_Gamma22,
	DISPLAYMODE_FSHDR_SCRGB,
	DISPLAYMODE_HDR10_2084,
	DISPLAYMODE_HDR10_SCRGB
};

DXGI_FORMAT fsHdrGetFormat(DisplayMode displayMode);