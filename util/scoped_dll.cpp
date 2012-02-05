#include "StdAfx.h"
#include "scoped_dll.h"
#include <pbc/buffer.h>
#include <logger.h>


#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;

scoped_dll::scoped_dll()
: m_handle(0)
{
}

scoped_dll::~scoped_dll()
{
	if (m_handle) {
		FreeLibrary(m_handle);
	}
}

void scoped_dll::load(const wstring& path, DWORD flags)
{
	m_handle = LoadLibraryExW(path.c_str(), NULL, flags);
	if (!m_handle) {
        string msg = string("Loading DLL failed for '") 
            + logger::string_cast<string>(path) + "': " 
            + logger::get_last_error();
        trace_log << msg << endl;
        throw std::runtime_error(msg);
	}
}
