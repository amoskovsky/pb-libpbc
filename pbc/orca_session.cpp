#include "StdAfx.h"
#include "orca_session.h"

#include <logger.h>

#define INIT_ORCA_METHOD(m) init_orca_method("PBORCA_" #m, (void**)&m)

namespace pbc {

using namespace std;

orca_session::orca_session( const wstring& pbrt_dll, int pb_ver, bool unicode )
: m_encoding(unicode ? BT_UTF16 : BT_ANSI)
, m_session(0)
{
    trace_log << "orca_session::orca_session " << pbrt_dll << endl;
    m_dll.load(pbrt_dll);
    trace_log << "loading methods" << endl;
    WALK_ORCA_METHODS(INIT_ORCA_METHOD);
    session_open();
}

orca_session::~orca_session()
{
    trace_log << "orca_session::~session_open" << endl;
    session_close();
}

void orca_session::session_open()
{
    trace_log << "orca_session::session_open" << endl;
    assert_throw(!m_session);
    m_session = SessionOpen();
    assert_throw(m_session);
}


void orca_session::session_close()
{
    if (m_session) {
        trace_log << "orca_session::session_close" << endl;
        SessionClose(m_session);
        m_session = 0;
    }
}

void orca_session::init_orca_method( const std::string& method_name, void ** method_addr )
{
    trace_log << "orca_session::init_orca_method " << method_name << endl;
    assert_throw(HMODULE(m_dll));
    FARPROC proc = GetProcAddress(m_dll, method_name.c_str());
    if (!proc) {
        throw orca_method_error("ORCA function " + method_name + " not found");
    }
    *method_addr = proc;

}

orca_error::orca_error( int error_code, const std::string& message )
: std::runtime_error(message)
, m_error_code(error_code)
{
}

} // namespace pbc

