#pragma once

#include <D3D12.h>
#include <string>

void SetName(ID3D12Object* pObj, const char* name);
void SetName(ID3D12Object* pObj, const std::string& name);
DXGI_FORMAT ConvertIntoGammaFormat(DXGI_FORMAT format);