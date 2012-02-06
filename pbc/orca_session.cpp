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
    m_lib_list = lib_list0;
    vector<TCHAR*> lib_list;
    lib_list.reserve(m_lib_list.size());
    BOOST_FOREACH(orca_string& lib, m_lib_list) {
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

void orca_session::set_current_app( orca_string lib, orca_string app )
{
    debug_log << "orca_session::set_current_app app=" << app.to_tstring() << " lib=" << lib.to_tstring() << endl;
    m_app_lib = lib;
    m_app = app;
    int err = SessionSetCurrentAppl(m_session, lib.make(m_encoding), app.make(m_encoding));
    if (err != PBORCA_OK)
        throw orca_error(err, get_error());
}

void WINAPI orca_session::compile_error_callback( PPBORCA_COMPERR err, LPVOID state )
{
    orca_session* session = (orca_session*)state;
    session->compile_error_callback(err);
}

void orca_session::compile_error_callback( PPBORCA_COMPERR err )
{
    orca_compile_error_item e;
    e.level = err->iLevel;
    e.line_number = err->iLineNumber;
    e.column_number = err->iColumnNumber;
    e.message_number = orca_string(m_encoding, err->lpszMessageNumber).to_tstring();
    e.message_text = orca_string(m_encoding, err->lpszMessageText).to_tstring();
    m_compile_errors.push_back(e);
}

int CompileEntryRegenerateWrapper(
    FP_CompileEntryRegenerate fn,
    HPBORCA        hORCASession,    
    LPTSTR          lpszLibraryName,
    LPTSTR          lpszEntryName,  
    PBORCA_TYPE    otEntryType,     
    PBORCA_ERRPROC pCompErrProc,    
    LPVOID         pUserData     
    ) 
{
    int ret = 0;
    __try {
        ret = fn(hORCASession, lpszLibraryName, lpszEntryName, otEntryType, pCompErrProc, pUserData);
    } __except (1) {
        ret = orca_compile_error::GPF_ERROR;
    }
    return ret;
}
void orca_session::compile_entry_regenerate( orca_string lib, orca_string entry, PBORCA_TYPE entry_type )
{
    debug_log << "orca_session::compile_entry_regenerate" 
        << " entry=" << entry.to_tstring() 
        << " entry_type=" << to_string(entry_type) 
        << " lib=" << lib.to_tstring() 
        << endl;
    m_compile_errors.clear();
    int err = CompileEntryRegenerateWrapper(
        CompileEntryRegenerate, 
        m_session, 
        lib.make(m_encoding), 
        entry.make(m_encoding),
        entry_type,
        &orca_session::compile_error_callback,
        this
        );
    if (err != PBORCA_OK) {
        if (err == orca_compile_error::GPF_ERROR) {
            recover();
            throw orca_compile_error(err, "memory access violation", m_compile_errors);
        }
        else {
            throw orca_compile_error(err, get_error(), m_compile_errors);
        }
    }
}

void orca_session::recover()
{
    debug_log << "orca_session::recover"  << endl;
    session_close();
    session_open();
    set_library_list(m_lib_list);
    set_current_app(m_app_lib, m_app);
}

orca_error::orca_error( int error_code, const std::string& message )
: std::runtime_error(message)
, m_error_code(error_code)
{
}


orca_compile_error::orca_compile_error( int error_code, const std::string& message, list<orca_compile_error_item>& error_items )
: orca_error(error_code, message)
, m_error_items(error_items)
{
}

std::string to_string( PBORCA_TYPE value )
{
    switch (value) {
    case PBORCA_APPLICATION: return "PBORCA_APPLICATION";     
    case PBORCA_DATAWINDOW: return "PBORCA_DATAWINDOW";      
    case PBORCA_FUNCTION: return "PBORCA_FUNCTION"; 
    case PBORCA_MENU: return "PBORCA_MENU"; 
    case PBORCA_QUERY: return "PBORCA_QUERY";     
    case PBORCA_STRUCTURE: return "PBORCA_STRUCTURE"; 
    case PBORCA_USEROBJECT: return "PBORCA_USEROBJECT"; 
    case PBORCA_WINDOW: return "PBORCA_WINDOW"; 
    case PBORCA_PIPELINE: return "PBORCA_PIPELINE"; 
    case PBORCA_PROJECT: return "PBORCA_PROJECT"; 
    case PBORCA_PROXYOBJECT: return "PBORCA_PROXYOBJECT"; 
    case PBORCA_BINARY: return "PBORCA_BINARY"; 
    default: return boost::lexical_cast<string>((int)value); 
    }
}


} // namespace pbc

