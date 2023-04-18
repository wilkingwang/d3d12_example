#include <TCHAR.H>
#include <minwindef.h>

#include "utils.h"

void string2tchar(const std::string& src, TCHAR* buf)
{
#ifdef UNICODE
	_stprintf_s(buf, MAX_PATH, _T("%S"), src.c_str());
#else
	_stprintf_s(buf, MAX_PATH, _T("%s"), src.c_str());
#endif
}