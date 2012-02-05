#include "StdAfx.h"
#include "orca_session.h"

#include <logger.h>

#include <boost/foreach.hpp>

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
    debug_log << "orca_session::session_open" << endl;
    assert_throw(!m_session);
    m_session = SessionOpen();
    assert_throw(m_session);
}


void orca_session::session_close()
{
    if (m_session) {
        debug_log << "orca_session::session_close" << endl;
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

void orca_session::set_library_list( std::vector<orca_string>& lib_list0 )
{
    debug_log << "orca_session::set_library_list " << lib_list0.size() << endl;
    assert_throw(!lib_list0.empty());
    vector<TCHAR*> lib_list;
    BOOST_FOREACH(orca_string& lib, lib_list0) {
        TCHAR* p = lib.make(m_encoding);
        assert_throw(p);
        debug_log << "library: " << lib.to_tstring() << endl;
        lib_list.push_back(p);
    }
    int err = SessionSetLibraryList(m_session, &lib_list[0], lib_list.size());
    if (err != PBORCA_OK)
        throw orca_error(err, get_error());
}

std::string orca_session::get_error()
{
    orca_string msg(m_encoding, 1024);
    SessionGetError(m_session, msg.buf(), msg.size());
    return msg.make_ansi();
}

void orca_session::set_current_app( orca_string app, orca_string lib )
{
    debug_log << "orca_session::set_current_app app=" << app.to_tstring() << " lib=" << lib.to_tstring() << endl;
    int err = SessionSetCurrentAppl(m_session, lib.make(m_encoding), app.make(m_encoding));
    if (err != PBORCA_OK)
        throw orca_error(err, get_error());
}

orca_error::orca_error( int error_code, const std::string& message )
: std::runtime_error(message)
, m_error_code(error_code)
{
}

} // namespace pbc

