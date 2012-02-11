#pragma once

#include <string>
#include <tchar.h>


#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

