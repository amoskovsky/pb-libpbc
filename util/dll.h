#pragma once

#include <windows.h>

#include <string>


class dll
{
public:
	dll();
	~dll();
    void load(const std::wstring& path, DWORD flags = LOAD_WITH_ALTERED_SEARCH_PATH);
	operator HMODULE() const { return m_handle; }
private:
	HMODULE m_handle;
};
