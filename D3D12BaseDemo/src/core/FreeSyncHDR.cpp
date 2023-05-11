#include "FreeSyncHDR.h"

DXGI_FORMAT fsHdrGetFormat(DisplayMode displayMode)
{
    switch (displayMode)
    {
    case DISPLAYMODE_SDR:
        return DXGI_FORMAT_R8G8B8A8_UNORM;

    case DISPLAYMODE_FSHDR_Gamma22:
        return DXGI_FORMAT_R10G10B10A2_UNORM;

    case DISPLAYMODE_FSHDR_SCRGB:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;

    case DISPLAYMODE_HDR10_2084:
        return DXGI_FORMAT_R10G10B10A2_UNORM;

    case DISPLAYMODE_HDR10_SCRGB:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;

    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}